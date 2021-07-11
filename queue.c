#include "queue.h"

/*********************************************
 * RequestInfo implementation
 ********************************************/

RequestInfo *create_info(int fd, struct timeval *arrival_time) {
    RequestInfo *info = (RequestInfo *) malloc(sizeof (RequestInfo));
    info->fd = fd;
    info->arrival_time.tv_usec = arrival_time->tv_usec;
    info->arrival_time.tv_sec = arrival_time->tv_sec;
    info->is_static_request = -1;
    return info;
}

void destroy_info(RequestInfo *info) {
    free(info);
}

/*********************************************
 * Node implementation
 ********************************************/

Node *create_node(int fd, struct timeval *arrival_time) {
    Node *node = (Node *) malloc((sizeof (Node)));
    if (node == NULL) return NULL;
    node->info = create_info(fd, arrival_time);
    node->next = NULL;
    node->previous = NULL;
    return node;
}

void destroy_node(Node *node) {
    destroy_info(node->info);
    free(node);
}

/*********************************************
 * List implementation
 ********************************************/

List *create_list() {
    List *list = (List *) malloc((sizeof (List)));
    if (list == NULL) return NULL;
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void *destroy_list(List * list) {
    Node *current_node = list->head;
    while (current_node != NULL) {
        Node *next_node_save = current_node->next;
        destroy_node(current_node);
        current_node = next_node_save;
    }
    free(list);
}

void add_node (List *list , int fd, struct timeval *arrival_time) {
    if (list == NULL) return;
    Node* new_node = create_node(fd, arrival_time);
    if (new_node == NULL) return;
    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;

    } else {
        list->tail->previous = new_node;
        new_node->next = list->tail;
        list->tail = new_node;
    }
}

void remove_node (List *list, Node * node) {
    if (node->previous != NULL) {
        node->previous->next = node->next;
    } else {
        list->tail = node->next;
    }
    if (node->next != NULL) {
        node->next->previous = node->previous;
    } else {
        list->head = node->previous;
    }
    destroy_node(node);
}

void remove_tail(List *list) {
    Node *new_tail_save = list->tail->previous;
    if (new_tail_save != NULL) {
        new_tail_save->next = NULL;
    }
    else {
        list->head = NULL;
    }
    destroy_node(list->tail);
    list->tail = new_tail_save;
}

void remove_head(List *list) {
    Node *new_head_save = list->head->previous;
    if (new_head_save != NULL) {
        new_head_save->next = NULL;
    }
    else {
        list->tail = NULL;
    }
    destroy_node(list->head);
    list->head = new_head_save;
}

/*********************************************
 * Queue implementation
 ********************************************/

Queue *create_queue(int queue_size, char *schedalg) {
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->requests = create_list();
    queue->queue_size = queue_size;
    queue->running_requests = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->condition, NULL);
    queue->overload_policy = (char *) malloc(sizeof (char) * strlen(schedalg) + 1);
    strcpy(queue->overload_policy, schedalg);
    return queue;
}

void queue_destroy(Queue * queue ) {
    pthread_cond_destroy(&queue->condition);
    pthread_mutex_destroy(&queue->mutex);
    destroy_list(queue->requests);
    free(queue->overload_policy);
    free(queue);
}

RequestInfo *queue_pop(Queue * queue) {

    pthread_mutex_lock(&queue->mutex);

    while (queue->requests->size == 0) {
        pthread_cond_wait(&queue->condition, &queue->mutex);
    }
    RequestInfo *info = (RequestInfo *) malloc(sizeof (RequestInfo));
    info->fd = queue->requests->head->info->fd;
    info->arrival_time = queue->requests->head->info->arrival_time;
    remove_head(queue->requests);
    queue->requests->size--;
    queue->running_requests++;
    pthread_cond_signal(&queue->condition);

    pthread_mutex_unlock(&queue->mutex);

    return info;
}

void queue_push(Queue * queue , int fd , struct timeval *arrival_time){

    pthread_mutex_lock(&queue->mutex);

    if (queue->requests->size + queue->running_requests >= queue->queue_size) {
        if (!strcmp(queue->overload_policy,"block")) {
            while (queue->requests->size + queue->running_requests >= queue->queue_size) {
                pthread_cond_wait(&queue->condition, &queue->mutex);
            }
            add_node(queue->requests, fd , arrival_time);
            queue->requests->size++;
        }

        else if (!strcmp(queue->overload_policy,"dt")) {
            Close(fd);
        }

        else if (!strcmp(queue->overload_policy,"dh")) {
            if (queue->running_requests != queue->queue_size) {
                Close(queue->requests->head->info->fd);
                remove_head(queue->requests);
                add_node(queue->requests, fd, arrival_time);
            }
            else {
                Close(fd);
            }
        }

        else if (!strcmp(queue->overload_policy,"random")) {
            if (queue->running_requests != queue->queue_size) {
                int request_to_remove;
                int number_of_requests_to_remove = ceil((double) queue->requests->size * 0.25);
                Node *current_node;
                for (int i = 0; i < number_of_requests_to_remove; i++) {
                    srand(time(NULL));
                    request_to_remove = rand() % queue->requests->size;
                    current_node = queue->requests->tail;
                    for (int j = 0; j < request_to_remove; j++) {
                        current_node = current_node->next;
                    }
                    Close(current_node->info->fd);
                    remove_node(queue->requests, current_node);
                    queue->requests->size--;
                }
                add_node(queue->requests, fd , arrival_time);
                queue->requests->size++;
            } else {
                Close(fd);
            }
        }
    }
    else {
        add_node(queue->requests , fd, arrival_time);
        queue->requests->size++;
    }
    pthread_cond_signal(&queue->condition);
    pthread_mutex_unlock(&queue->mutex);

}