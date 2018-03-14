#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include"./protocol.h"
#include<unistd.h>
#include<vector>
#include<algorithm>
#include<errno.h>
#include<sys/epoll.h>
#include<pthread.h>
#define ERROR_EXIT(msg) \
        do{ perror(msg); exit(EXIT_FAILURE); }while(0)
using namespace std;
namespace Network
{
    static const int MAX_CLIENT_CONN = 1024;
    //保存全部连接
    static vector<int> clients(MAX_CLIENT_CONN);
    //处理epoll事件的动态数组
    typedef vector<struct epoll_event> EpollList;
    //初始化socket
    int create_tcp_socket(short port);
    //以ET模式添加事件
    void add_to_epoll(int epfd, struct epoll_event* ev, int fd);
};
void Network::add_to_epoll(int epfd, struct epoll_event* ev, int fd)
{
    ev->data.fd = fd;
    ev->events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, ev);
}

int Network::create_tcp_socket(short port)
{
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd < 0)
        ERROR_EXIT("socket");
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons((short)port);
    myaddr.sin_addr.s_addr = INADDR_ANY;
    int flag = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
        ERROR_EXIT("setsockopt");
    if(bind(listenfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0)
        ERROR_EXIT("bind");
    if(listen(listenfd, 128) < 0)
        ERROR_EXIT("listen");
    return listenfd;
}

int main(int argc,char** argv)
{
    using namespace Network;
    packet_t recv_pkg;
    bzero(&recv_pkg,sizeof(recv_pkg));
    int listenfd = create_tcp_socket((short)atoi(argv[1]));
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    struct epoll_event ev;
    add_to_epoll(epfd, &ev, listenfd);
    EpollList el(MAX_CLIENT_CONN);
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    int ready;
    while(1)
    {
        ready = epoll_wait(epfd, &*el.begin(), MAX_CLIENT_CONN, -1);
        if(ready == -1)
            ERROR_EXIT("epoll_wait");
        for(int i = 0; i < ready; i++)
        {
            if(el[i].data.fd == listenfd)
            {
               //用accept4可以省去设置fd nonblocking属性的步骤
                int conn = accept4(listenfd, (struct sockaddr*)&peer, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
                if(conn == -1)
                {
                    if(errno == EINTR)
                        continue;
                    ERROR_EXIT("accept4");
                }
                printf("client %s connected\n",inet_ntoa(peer.sin_addr));
                add_to_epoll(epfd, &ev, conn);
                clients.push_back(conn);
            }
            else if(el[i].data.fd & EPOLLIN)
            {
                int s = read(el[i].data.fd, &recv_pkg.length, 4); 
                if(s == 0)
                {
                    printf("client logoff\n");
                    close(el[i].data.fd);
                    clients.erase(find(clients.begin(), clients.end(), el[i].data.fd));
                    epoll_ctl(epfd, EPOLL_CTL_DEL, el[i].data.fd, &el[i]);
                }
                else if(s < 0)
                {
                    if(errno == EINTR)
                        continue;
                    // 由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读
                    // 在这里就当作是该次事件已处理处
                    if(errno == EAGAIN)
                        continue;
                    ERROR_EXIT("read length");
                }
                else
                {
                    int ss = ntohl(recv_pkg.length);
                    int pkgsize = read(el[i].data.fd, recv_pkg.content, ss);
                    if(pkgsize < 0 )
                    {
                        if(errno == EINTR)
                            continue;
                        if(errno == EAGAIN)
                            continue;
                        ERROR_EXIT("read content");
                    }
                    else if(pkgsize == 0)
                    {
                        printf("client logoff\n");
                        close(el[i].data.fd);
                        clients.erase(find(clients.begin(), clients.end(), el[i].data.fd));
                        epoll_ctl(epfd, EPOLL_CTL_DEL, el[i].data.fd, &el[i]);
                    }
                    else
                    {
                        printf("content is %s\n",recv_pkg.content);
                        write(el[i].data.fd, &recv_pkg, ss + 4);
                        memset(&recv_pkg, 0, sizeof(recv_pkg));
                    }
                }
                if(--ready <= 0)
                    continue;
            }
        }
    }
    close(listenfd);
    close(epfd);
}
