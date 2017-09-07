#include"./client_list.h"
#include<stdlib.h>
struct client_list* initList()
{
    struct client_list* head = (struct client_list*)malloc(sizeof(struct client_list));
    head->next = NULL;
    head->confd = -1;
    return head;
}
int deleteElement(struct client_list* head, struct client_list* target)
{
    if(!target || !head)
        return -1;
    struct client_list* temp = head;
    while(temp)
    {
        struct client_list* temp2 = temp->next;
        if(temp2 && (target->confd == temp2->confd))
        {
            temp->next = temp->next->next;
            free(temp2);
            temp2 = NULL;
        }
        temp = temp->next;
    }
    return 0;
}
int insertFromTail(struct client_list* head, struct client_list* target)
{
    if(!target || !head)
        return -1;
    struct client_list* temp = head;
    while(temp)
    {
        if(temp->next)
        {
            temp = temp->next;
        }
        else
        {
            struct client_list* temp2 = (struct client_list*)malloc(sizeof(struct client_list));
            temp2->next = NULL;
            temp2->confd = target->confd;
            temp->next = temp2;
            break;
        }
    }
    return 0;
}

unsigned elementCount(struct client_list* head)
{
    if(!head || !(head->next))
        return 0;
    struct client_list* temp = head->next;
    unsigned count = 0;
    while(temp)
    {
        count++;
        temp = temp->next;
    }
    return count;
}
