#include "queue.h"

/*********************************************
 * Node implementation
 ********************************************/

Node *create_node(int val) {
    Node *node = (Node *) malloc((sizeof (Node)));
    if (node == NULL) return NULL; //TODO: error handling
    node->val = val;
    node->next = NULL;
    node->previous = NULL;
    return node;
}

void *destroy_node(Node *node) {
    free(node);
}

unsigned int get_node_value(Node *node) {
    return node->val;
}

/*********************************************
 * List implementation
 ********************************************/

List *create_list() {
    List *list = (List *) malloc((sizeof (List)));
    if (list == NULL) return NULL; //TODO: error handling
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void *destroy_list(List * list) {
    Node *current_node = list->head;
    while (current_node != NULL) {
        Node *next_node_save = current_node->next;
        free(current_node);
        current_node = next_node_save;
    }
    free(list);
}

void add_node (List *list , int val) {
    if (list == NULL) return;
    Node* new_node = create_node(val);
    if (new_node == NULL) return;
    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;

    } else {
        list->head->previous = new_node;
        new_node->next = list->head;
        list->head = new_node;
    }
}

void remove_node (List *list, int val) {
    Node *current_node = list->head;
    while (current_node != NULL) {
        if (current_node->val == val) {
            current_node->previous->next = current_node->next;
            current_node->next->previous = current_node->previous;
            free(current_node);
            break;
        }
        current_node = current_node->next;
    }
}

void remove_tail(List *list) {
    Node *new_tail_save = list->tail->previous;
    new_tail_save->next = NULL;
    free(list->tail);
    list->tail = new_tail_save;
}

void remove_head(List *list) {
    Node *new_head_save = list->head->next;
    new_head_save->previous = NULL;
    free(list->head);
    list->head = new_head_save;
}

/*********************************************
 * Queue implementation
 ********************************************/

Queue *create_queue(int queue_size) {
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->requests = create_list();
    queue->queue_size = queue_size;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->condition, NULL);
//    if(sem_init(queue->mutex , 0 , 1) == -1) return NULL; //TODO: error handling
//    if(sem_init(queue->items , 0 , 0) == -1) return NULL; //TODO: error handling
//    if(sem_init(queue->spaces , 0 , queue_size-1) == -1) return NULL; //TODO: error handling
    return queue;
}

void queue_destroy(Queue * queue ) {
//    sem_destroy(queue->spaces);
//    sem_destroy(queue->items);
//    sem_destroy(queue->mutex);
    pthread_cond_destroy(&queue->condition);
    pthread_mutex_destroy(&queue->mutex);
    free(queue->requests);
    free(queue);
}

int queue_pop(Queue * queue) {
//    sem_wait(queue->items);
//    sem_wait(queue->mutex);

    pthread_mutex_lock(&queue->mutex);

    // TODO: will need modifications for part 2

//    if (queue->requests->tail == NULL) {
//        sem_post(queue->mutex);
//        sem_post(queue->spaces);
//        return -1;
//    }
    while (queue->requests->size == 0) {
        pthread_cond_wait(&queue->condition, &queue->mutex);
    }
    int fd = queue->requests->tail->val;
    remove_tail(queue->requests);
    queue->requests->size--;
    pthread_cond_signal(&queue->condition);

    pthread_mutex_unlock(&queue->mutex);
//    sem_post(queue->mutex);
//    sem_post(queue->spaces);
    return fd;
}

void queue_push(Queue * queue , int fd, char *schedalg){
//    sem_wait(queue->spaces);
//    sem_wait(queue->mutex);
    pthread_mutex_lock(&queue->mutex);

    if (!strcmp(schedalg,"block")) {
        while (queue->requests->size > queue->queue_size) {
            pthread_cond_wait(&queue->condition, &queue->mutex);
        }
        add_node(queue->requests, fd);
        queue->requests->size++;
        pthread_cond_signal(&queue->condition);
    }

    else if (!strcmp(schedalg,"dt")) {
        Close(fd);
    }

    else if (!strcmp(schedalg,"dh")) {
        remove_head(queue->requests);
        add_node(queue->requests, fd);
        pthread_cond_signal(&queue->condition);
    }

    pthread_mutex_unlock(&queue->mutex);
//    sem_post(queue->mutex);
//    sem_post(queue->items);
}