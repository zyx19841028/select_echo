#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/select.h>
#include"./protocol.h"
#define ERROR_EXIT(msg) \
        do{perror(msg);exit(EXIT_FAILURE);}while(0)

int main(int argc, char** argv)
{
    packet_t recv_buf;
    packet_t send_buf;
    bzero(&recv_buf, sizeof(recv_buf));
    bzero(&recv_buf, sizeof(send_buf));

    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd < 0)
        ERROR_EXIT("socket");
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons((short)(atoi(argv[2])));
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    if(connect(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        ERROR_EXIT("connect");
    fd_set rset;
    FD_ZERO(&rset);
    int ready;
    int maxfd;
    int input_fd = fileno(stdin);//将stdin转为文件描述符，以防stdin被重定向
    maxfd = (input_fd > fd) ? input_fd : fd;
    while(1)
    {
        FD_SET(input_fd, &rset);//需要重新将fd放入rset
        FD_SET(fd, &rset);
        ready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if(ready == -1)
            ERROR_EXIT("select");
        if(ready == 0)
            continue;
        if(FD_ISSET(input_fd, &rset))
        {
            if(fgets(send_buf.content, SIZE, stdin) == NULL)
                break;
            int s = strlen(send_buf.content);
            send_buf.length = htonl(s); 
            if(write(fd, &send_buf, 4+s) == -1)
                ERROR_EXIT("write");
            memset(&send_buf, 0, sizeof(send_buf));
        }
        if(FD_ISSET(fd, &rset))
        {
            int s = read(fd, &recv_buf.length, 4);
            if(s < 0)
                ERROR_EXIT("read");
            if(s == 0)
            {
                printf("server disconnected");
                break;
            }
            recv_buf.length = ntohl(recv_buf.length);
            int s2 = read(fd, recv_buf.content, recv_buf.length);
            if(s2 < 0)
                ERROR_EXIT("read");
            if(s2 == 0)
            {
                printf("server disconnected");
                break;
            }
            fputs(recv_buf.content, stdout);
            memset(&recv_buf, 0, sizeof(recv_buf));
        }
    }
    close(fd);
}
