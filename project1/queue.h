#ifndef _QUEUE_H
#define _QUEUE_H
#define MAX 300

typedef struct QUEUE
{
    int pidArray[MAX];
    int front,rear,item_count;
}queue;

void q_init(queue* q);
int q_peek(queue* q);
int q_isEmpty(queue* q);
int q_isFull(queue* q);
int q_size(queue* q);
int q_insert(queue* q,int pid);
int q_remove(queue* q);

#endif