#include "queue.h"

/*********************************************
 * RequestInfo implementation
 ********************************************/

RequestInfo *create_info(int fd, double arrival_time) {
    RequestInfo *info = (RequestInfo *) malloc(sizeof (RequestInfo));
    info->fd = fd;
    info->arrival_time = arrival_time;
    info->dispatch_time = -1;
    info->is_static_request = -1;
}

void destroy_info(RequestInfo *info) {
    free(info);
}

/*********************************************
 * Node implementation
 ********************************************/

Node *create_node(int fd, double arrival_time) {
    Node *node = (Node *) malloc((sizeof (Node)));
    if (node == NULL) return NULL; //TODO: error handling
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
        destroy_node(current_node);
        current_node = next_node_save;
    }
    free(list);
}

void add_node (List *list , int fd, double arrival_time) {
    if (list == NULL) return;
    Node* new_node = create_node(fd, arrival_time);
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

void remove_node (List *list, int fd, double arrival_time) {
    Node *current_node = list->head;
    while (current_node != NULL) {
        if (current_node->info->fd == fd && current_node->info->arrival_time == arrival_time) {
            current_node->previous->next = current_node->next;
            current_node->next->previous = current_node->previous;
            destroy_node(current_node);
            break;
        }
        current_node = current_node->next;
    }
}

void remove_tail(List *list) {
    Node *new_tail_save = list->tail->previous;
    new_tail_save->next = NULL;
    destroy_node(list->tail);
    list->tail = new_tail_save;
}

void remove_head(List *list) {
    Node *new_head_save = list->head->next;
    new_head_save->previous = NULL;
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

    // TODO: will need modifications for part 2

    while (queue->requests->size == 0) {
        pthread_cond_wait(&queue->condition, &queue->mutex);
    }
    RequestInfo *info = (RequestInfo *) malloc(sizeof (RequestInfo));
    info->fd = queue->requests->tail->info->fd;
    info->arrival_time = queue->requests->tail->info->arrival_time;
    info->dispatch_time =  -1;
    remove_tail(queue->requests);
    queue->requests->size--;
    pthread_cond_signal(&queue->condition);

    pthread_mutex_unlock(&queue->mutex);

    return info;
}

void queue_push(Queue * queue , int fd , double arrival_time){

    pthread_mutex_lock(&queue->mutex);

    if (queue->requests->size > queue->queue_size) {
        if (!strcmp(queue->overload_policy,"block")) {
            while (queue->requests->size > queue->queue_size) {
                pthread_cond_wait(&queue->condition, &queue->mutex);
            }
            add_node(queue->requests, fd , arrival_time);
            queue->requests->size++;
            pthread_cond_signal(&queue->condition);
        }

        else if (!strcmp(queue->overload_policy,"dt")) {
            Close(fd);
        }

        else if (!strcmp(queue->overload_policy,"dh")) {
            remove_head(queue->requests);
            add_node(queue->requests, fd, arrival_time);
            pthread_cond_signal(&queue->condition);
        }
    }
    else {
        add_node(queue->requests , fd , arrival_time );
        queue->requests->size++;
        pthread_cond_signal(&queue->condition);
    }

    pthread_mutex_unlock(&queue->mutex);

}