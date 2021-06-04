#include "queue.h"

/*********************************************
 * Node implementation
 ********************************************/

Node *create_node(unsigned int val) {
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

void add_node (List *list , unsigned int val) {
    if (list == NULL) return;
    Node* new_node = create_node(val);
    if (new_node == NULL) return;
    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;

    } else {
        list->tail->next=new_node;
        new_node->previous=list->tail;
        list->tail = new_node;
    }
}

void remove_node (List *list, unsigned int val) {
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

/*********************************************
 * Queue implementation
 ********************************************/

Queue *create_queue(unsigned int queue_size) {
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->requests = create_list();
    queue->queue_size = queue_size;
    if(sem_init(queue->mutex , 0 , 1) == -1) return NULL; //TODO: error handling
    if(sem_init(queue->items , 0 , 0) == -1) return NULL; //TODO: error handling
    if(sem_init(queue->spaces , 0 , queue_size-1) == -1) return NULL; //TODO: error handling
    return queue;
}

void queue_destroy(Queue * queue ) {
    sem_destroy(queue->spaces);
    sem_destroy(queue->items);
    sem_destroy(queue->mutex);
    free(queue->requests);
    free(queue);
}

void queue_pop(Queue * queue){
    sem_wait(queue->items);
    sem_wait(queue->mutex);
        // remove request
    queue->requests->size--;
    sem_post(queue->mutex);
    sem_post(queue->spaces);

}
void queue_push(Queue * queue , Node * item){
    sem_post(queue->spaces);
    sem_post(queue->mutex);
        // add request
    queue->requests->size++;
    sem_wait(queue->mutex);
    sem_wait(queue->items);
}