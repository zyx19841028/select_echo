#ifndef CLIENT_LIST
#define CLIENT_LIST

struct client_list
{
    int confd;
    struct client_list* next;
};
/*文件描述符链表初始化*/
struct client_list* initList();
/*从文件描述符链表尾插入*/
int insertFromTail(struct client_list* head, struct client_list* target);
/*删除对象*/
int deleteElement(struct client_list* head, struct client_list* target);
/*返回文件描述符链表中元素个数*/
unsigned elementCount(struct client_list* head);
#endif
