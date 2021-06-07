#include "segel.h"

#ifndef OS234123_HW3_WORKER_THREAD_H
#define OS234123_HW3_WORKER_THREAD_H

typedef struct worker_thread_t {
  pthread_t thread;
  int busy;
} worker_thread ;

worker_thread* worker_thread_create_node();

worker_thread** worker_thread_create(int num_of_threads);

//int get_busy(worker_thread worker);

#endif //OS234123_HW3_WORKER_THREAD_H
