#include "queue.h"

void q_init(queue* q)
{
    q->front = 0;
    q->rear = -1;
    q->item_count = 0;
}

int q_peek(queue* q)
{
    if(q->item_count > 0)
        return q->pidArray[q->front];
    else
        return -1;
}

int q_isEmpty(queue* q)
{
    return (q->item_count==0);
}

int q_isFull(queue* q)
{
    return (q->item_count == MAX);
}

int q_size(queue* q)
{
    return q->item_count;
}

int q_insert(queue* q,int pid)
{
    if(q_isFull(q) == 0) {
	
        if(q->rear == MAX-1) {
             q->rear = -1;            
        }       

        q->pidArray[++q->rear] = pid;
        q->item_count++;
        return 0;
   }else
        return -1;  
}

int q_remove(queue* q)
{
    if(q_isEmpty(q)==1)
        return -1;

    int data = q->pidArray[q->front++];
	
    if(q->front == MAX) {
        q->front = 0;
    }
	
   q->item_count--;
   return data; 
}
