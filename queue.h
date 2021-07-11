#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "segel.h"

/*********************************************
 * RequestInfo declaration
 ********************************************/

typedef struct {
    int fd;
    struct timeval arrival_time;
    struct timeval dispatch_time;
    int is_static_request;
} RequestInfo;

RequestInfo *create_info(int fd, struct timeval *arrival_time);

void destroy_info(RequestInfo *info);

/*********************************************
 * Node declaration
 ********************************************/

typedef struct Node_t {
   RequestInfo * info;
   struct Node_t * next;
   struct Node_t * previous;
} Node;

Node *create_node(int fd, struct timeval *arrival_time);

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

void add_node (List *list , int fd, struct timeval *arrival_time);

void remove_node (List *list, Node * node);

void remove_tail(List *list);

void remove_head(List *list);

/*********************************************
 * Queue declaration
 ********************************************/

typedef struct {
    List * requests;
    int queue_size; // total number of requests allowed (running + waiting)
    // added
    int running_requests;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    char *overload_policy;
} Queue;

Queue *create_queue(int queue_size, char *schedalg);

void queue_destroy(Queue * queue);

RequestInfo *queue_pop(Queue * queue);

void queue_push(Queue * queue , int fd, struct timeval *arrival_time);

#endif