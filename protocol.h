#ifndef _PROTOCOL_H
#define _PROTOCOL_H
#define SIZE 1024
typedef struct packet
{
    int length;
    char content[SIZE];
}packet_t;
#endif
