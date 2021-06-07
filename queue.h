#include "segel.h"

/*********************************************
 * RequestInfo declaration
 ********************************************/

typedef struct {
    int fd;
    double arrival_time;
    double dispatch_time;
    int is_static_request;
} RequestInfo;

RequestInfo *create_info(int fd, double arrival_time);

void destroy_info();

/*********************************************
 * Node declaration
 ********************************************/

typedef struct Node_t {
   RequestInfo * info;
   struct Node_t * next;
   struct Node_t * previous;
} Node;

Node *create_node(int fd, double arrival_time);

void destroy_node(Node *node);

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

void add_node (List *list , int fd, double arrival_time);

void remove_node (List *list, int fd, double arrival_time);

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
    char *overload_policy;
//    sem_t  *mutex;
//    sem_t  *items; // running requests
//    sem_t  *spaces; // spaces for more requests
} Queue;

Queue *create_queue(int queue_size, char *schedalg);

void queue_destroy(Queue * queue);

RequestInfo *queue_pop(Queue * queue);

void queue_push(Queue * queue , int fd, double arrival_time);