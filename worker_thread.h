#include "segel.h"
#include "queue.h"
#ifndef OS234123_HW3_WORKER_THREAD_H
#define OS234123_HW3_WORKER_THREAD_H

typedef struct worker_thread_t {
  pthread_t thread;
  int busy;
} worker_thread ;

worker_thread* worker_thread_create_node(){
    worker_thread* new_worker = (worker_thread*)malloc(sizeof(worker_thread));
    new_worker->busy=0;
    return new_worker;
}
worker_thread** worker_thread_create(int num_of_threads){

    worker_thread **threads = (worker_thread**) malloc(sizeof(worker_thread*) * num_of_threads);
    for(int i = 0 ;i<num_of_threads;i++){
        threads[i] = worker_thread_create_node();
    }
    return threads;
}

int get_busy(worker_thread worker){

    return worker.busy;
}


#endif //OS234123_HW3_WORKER_THREAD_H
