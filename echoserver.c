#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include"./protocol.h"
#include"./client_list.h"
#include<unistd.h>
#define ERROR_EXIT(msg) \
        do{ perror(msg); exit(EXIT_FAILURE); }while(0)
int main(void)
{
    packet_t recv_pkg;
    bzero(&recv_pkg,sizeof(recv_pkg));
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd < 0)
        ERROR_EXIT("socket");
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons((short)9877);
    myaddr.sin_addr.s_addr = INADDR_ANY;
    int flag = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
        ERROR_EXIT("setsockopt");
    if(bind(listenfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0)
        ERROR_EXIT("bind");
    if(listen(listenfd, 128) < 0)
        ERROR_EXIT("listen");

    int maxfd = listenfd;
    int acceptfd;
    fd_set all,temp;
    FD_ZERO(&all);
    FD_ZERO(&temp);
    FD_SET(listenfd,&all);
    int ready;
    struct client_list* cl = initList(); 
    while(1)
    {
        temp = all;
        ready = select(maxfd + 1, &temp, NULL, NULL, NULL);
        if(ready == -1)
            ERROR_EXIT("select");
        if(FD_ISSET(listenfd, &temp))
        {
            acceptfd = accept(listenfd, NULL, NULL);
            if(acceptfd < 0)
                ERROR_EXIT("accept");
            struct client_list* element = (struct client_list*)malloc(sizeof(struct client_list));
            element->confd = acceptfd;
            element->next = NULL;
            insertFromTail(cl, element);
            if(elementCount(cl) >= FD_SETSIZE)
                ERROR_EXIT("Too many clients");
            FD_SET(acceptfd, &all);
            if(acceptfd > maxfd)
                maxfd = acceptfd;
            if(--ready <= 0)
                continue;
        }
        struct client_list* ptr = cl->next;
        while(ptr)
        {
            if(FD_ISSET(ptr->confd, &temp))
            {
                int s = read(ptr->confd, &recv_pkg.length, 4); 
                if(s == 0)
                {
                    printf("client logoff\n");
                    close(ptr->confd);
                    FD_CLR(ptr->confd, &all);
                    deleteElement(cl, ptr);    
                }
                else if(s < 0)
                    ERROR_EXIT("read length");
                else
                {
                    int ss = 0;
                    ss = ntohl(recv_pkg.length);
                    int pkgsize = read(ptr->confd, recv_pkg.content, ss);
                    if(pkgsize < 0 )
                        ERROR_EXIT("read content");
                    else if(pkgsize == 0)
                    {
                        printf("Client logoff\n");
                        close(ptr->confd);
                        FD_CLR(ptr->confd, &all);
                        deleteElement(cl, ptr);    
                    }
                    else
                    {
                        printf("content is %s",recv_pkg.content);
                        write(ptr->confd, &recv_pkg, ss + 4);
                        memset(&recv_pkg, 0, sizeof(recv_pkg));    
                    }
                }
                if(--ready <= 0)
                    break;
            }
            ptr = ptr->next;
        }
    }
}
