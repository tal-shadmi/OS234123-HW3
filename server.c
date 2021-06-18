#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "segel.h"
#include "queue.h"
#include "request.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// pthread_mutex_t server_respond_mutex = PTHREAD_MUTEX_INITIALIZER;

// HW3: Parse the new arguments too
void getargs(int *port,int *num_of_threads,int *queue_size ,char **schedalg, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *num_of_threads = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    *schedalg = (char *) malloc(sizeof(char) * strlen(argv[4]) + 1);
    strcpy(*schedalg, argv[4]);
}

double Time_GetMiliSeconds() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) ((double)t.tv_sec + (double)t.tv_usec / 1e6);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void check_for_requests(ServerInfo *server_info) {
    while (1) {
        RequestInfo *request_info = queue_pop(server_info->requests_queue);
        struct timeval current_time;
        gettimeofday(&current_time,NULL);
        request_info->dispatch_time.tv_usec = current_time.tv_usec - request_info->arrival_time.tv_usec;
        request_info->dispatch_time.tv_sec = current_time.tv_sec - request_info->arrival_time.tv_sec;
        requestHandle(server_info, request_info);
        Close(request_info->fd);
        destroy_info(request_info);
    }
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen , num_of_threads , queue_size;
    char *schedalg;
    struct sockaddr_in clientaddr;

    getargs(&port, &num_of_threads, &queue_size, &schedalg, argc, argv);

    // 
    // HW3: Create some threads...
    //

    worker_thread **threads = worker_thread_create(num_of_threads);
    Queue *requests_queue = create_queue(queue_size, schedalg);

    for (int i = 0 ; i < num_of_threads ; i++) {
        ServerInfo *server_info = (ServerInfo *) malloc(sizeof (ServerInfo));
        server_info->thread_id = i;
        server_info->thread_pool = threads;
        server_info->requests_queue = requests_queue;
        if (pthread_create(&threads[i]->thread, NULL, (void *)check_for_requests, server_info)) {
            printf("error");
        }
    }

    listenfd = Open_listenfd(port);

    while (1) {

	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        struct timeval *arrival_time = (struct timeval *) malloc(sizeof (struct timeval));
        gettimeofday(arrival_time,NULL);
        queue_push(requests_queue, connfd, arrival_time);

	// 
	// HW3: In general, don't handle these requests in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work
	//

    }

}
#pragma clang diagnostic pop

#endif


 
