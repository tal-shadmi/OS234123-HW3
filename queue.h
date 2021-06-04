#include "segel.h"


typedef struct {
   unsigned int val;

   Node * next;
   Node * previous;
}Node;


typedef struct {
    unsigned int size;
    Node * head;
    Node * tail;
}List;

typedef struct {
    List * requests;
    int queue_size;
    sem_t  *mutex;
    sem_t  *items; //current running
    sem_t  *spaces; //
} Queue;

Queue *make_queue(unsigned int queue_size)
{
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->queue_size = queue_size;
    if(sem_init(queue->mutex , 0 , 1)==-1) return NULL;//TODO error handling
    if(sem_init(queue->items , 0 , 0)==-1)return NULL;
    if(sem_init(queue->spaces , 0 , queue_size-1)==-1)return NULL;
    return queue;
}

void queue_destroy(Queue * queue ){
    sem_destroy(queue->spaces);
    sem_destroy(queue->items);
    sem_destroy(queue->mutex);
}
void queue_pop(Queue * queue);
void queue_push(Queue * queue , Node * item);


