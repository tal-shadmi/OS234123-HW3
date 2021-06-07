#include "segel.h"

/*********************************************
 * Node declaration
 ********************************************/

typedef struct Node_t {
   int val;
   struct Node_t * next;
   struct Node_t * previous;
} Node;

Node *create_node(int val);

void *destroy_node(Node *node);

unsigned int get_node_value(Node *node);

/*********************************************
 * List declaration
 ********************************************/

typedef struct {
    int size;
    Node * head;
    Node * tail;
} List;

List *create_list();

void *destroy_list(List * list);

void add_node (List *list , int val);

void remove_node (List *list, int val);

void remove_tail(List *list);

void remove_head(List *list);

/*********************************************
 * Queue declaration
 ********************************************/

typedef struct {
    List * requests;
    int queue_size; // total number of requests allowed
    pthread_mutex_t mutex;
    pthread_cond_t condition;
//    sem_t  *mutex;
//    sem_t  *items; // running requests
//    sem_t  *spaces; // spaces for more requests
} Queue;

Queue *create_queue(int queue_size);

void queue_destroy(Queue * queue);

int queue_pop(Queue * queue);

void queue_push(Queue * queue , int fd, char *schedalg);