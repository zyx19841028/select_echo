#ifndef QUEUE_H
#define QUEUE_H
typedef int DATATYPE;
struct node
{
    struct node* next;
    DATATYPE data;
};

struct queue
{
    struct node* head;
    struct node* tail;
    unsigned length;
};

int Enque(struct queue* q, DATATYPE element);
struct node* Deque(struct queue* q);
unsigned int QueueLength(struct queue* q);
void ClearQueue(struct queue* q);
struct queue* InitQueue();
int DeleteByVal(struct queue* q, DATATYPE val);
#endif
