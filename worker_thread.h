#ifndef OS234123_HW3_WORKER_THREAD_H
#define OS234123_HW3_WORKER_THREAD_H

#include "segel.h"

typedef struct {
  pthread_t thread;
  int id;
  int requests_count;
  int dynamic_requests_count;
  int static_requests_count;
} worker_thread;

worker_thread* worker_thread_create_node();

worker_thread** worker_thread_create(int num_of_threads);

//int get_busy(worker_thread worker);

#endif //OS234123_HW3_WORKER_THREAD_H
