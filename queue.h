#include "segel.h"

/*********************************************
 * Node declaration
 ********************************************/

typedef struct Node_t {
   unsigned int val;
   struct Node_t * next;
   struct Node_t * previous;
} Node;

Node *create_node(unsigned int val);

void *destroy_node(Node *node);

unsigned int get_node_value(Node *node);

/*********************************************
 * List declaration
 ********************************************/

typedef struct {
    unsigned int size;
    Node * head;
    Node * tail;
} List;

List *create_list();

void *destroy_list(List * list);

void add_node (List *list , unsigned int val);

void remove_node (List *list, unsigned int val);

/*********************************************
 * Queue declaration
 ********************************************/

typedef struct {
    List * requests;
    unsigned int queue_size; // total number of requests allowed
    sem_t  *mutex;
    sem_t  *items; // running requests
    sem_t  *spaces; // spaces for more requests
} Queue;

Queue *create_queue(unsigned int queue_size);

void queue_destroy(Queue * queue );

void queue_pop(Queue * queue);

void queue_push(Queue * queue , Node * item);


