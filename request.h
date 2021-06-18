#ifndef __SERVER_C__
#define __SERVER_C__

#include "queue.h"
#include "worker_thread.h"

typedef struct {
    Queue *requests_queue;
    worker_thread **thread_pool;
    int thread_id;
} ServerInfo;

void requestHandle(ServerInfo *server_info, RequestInfo *request_info);

#endif
