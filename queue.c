#include "./queue.h"
#include "stdlib.h"
struct queue* InitQueue()
{
    struct queue* q  = (struct queue*)malloc(sizeof(struct queue));
    q->head = q->tail = NULL;
    q->length = 0;
    return q;
}
int Enque(struct queue* q, DATATYPE element)
{
    if(!q)
        return -1;
    struct node* n = (struct node*)malloc(sizeof(struct node));
    n->next = NULL;
    n->data = element;
    if(!q->head)
    {
        q->head = q->tail = n;
        q->length += 1;
    }
    else
    {
        if(q->tail)
        {
            q->tail->next = n;
            q->tail = q->tail->next;
            q->length += 1;
        }
        else
            return -1;
    }
    return 0;
}
struct node* Deque(struct queue* q)
{
    if(!q || !q->head || !q->tail)
        return NULL;
    struct node* temp = q->head;
    q->head = q->head->next;
    q->length -= 1;
    return temp;
}
unsigned int QueueLength(struct queue* q)
{
    if(q)
        return q->length;
    else
        return 0;
}
void ClearQueue(struct queue* q)
{
    if(!q || !q->tail || !q->head)
       return;
    while(!q->head)
    {
        struct node* temp = q->head;
        q->head = q->head->next;
        free(temp);
        temp = NULL;
        q->length -= 1;
    }
    free(q);
    q = NULL;
}
int DeleteByVal(struct queue* q, DATATYPE val)
{
    if(!q || !q->head || !q->tail)
        return -1;
    struct queue* queue2 = InitQueue();
    unsigned length = 0;
    unsigned qlength = QueueLength(q);
    while(q->head != NULL)
    {
        struct node* n = Deque(q);
        if(n->data == val)
        {
            free(n);
            n = NULL;
            length++;
        }
        else
        {
            if(!queue2->head)
            {
                queue2->head = queue2->tail = n;
            }
            else
            {
                if(!queue2->tail)
                    return -1;
                else
                {
                    queue2->tail->next = n;
                    queue2->tail = queue2->tail->next;
                }
            }
        }
    }
    q->length = qlength - length;
    q->head = queue2->head;
    q->tail = queue2->tail;
    free(queue2);
    return  0;
}
