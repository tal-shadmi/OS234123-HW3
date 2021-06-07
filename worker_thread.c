#include "worker_thread.h"

worker_thread* worker_thread_create_node(){
    worker_thread* new_worker = (worker_thread*) malloc(sizeof(worker_thread));
    new_worker->requests_count = 0;
    new_worker->dynamic_requests_count = 0;
    new_worker->static_requests_count = 0;
    return new_worker;
}

worker_thread** worker_thread_create(int num_of_threads) {
    worker_thread **threads = (worker_thread **) malloc(sizeof (worker_thread*) * num_of_threads);
    for(int i = 0 ; i < num_of_threads ; i++){
        threads[i] = worker_thread_create_node();
        threads[i]->id = i;
    }
    return threads;
}