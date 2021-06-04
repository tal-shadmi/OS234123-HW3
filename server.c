#include "segel.h"
#include "request.h"
#include "worker_thread.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
void getargs(int *port,int *num_of_threads,int *queue_size ,char *schedalg, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *num_of_threads = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    schedalg = (char *) malloc(sizeof(char) * strlen(argv[4]));
    strcpy(schedalg, argv[4]);
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen , num_of_threads , queue_size;
    char *schedalg;
    struct sockaddr_in clientaddr;

    getargs(&port, &num_of_threads, &queue_size, schedalg, argc, argv);

    // 
    // HW3: Create some threads...
    //

    worker_thread **threads = worker_thread_create(num_of_threads);

    listenfd = Open_listenfd(port);

    while (1) {


	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

	// 
	// HW3: In general, don't handle these requests in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work
	//

        for (int i = 0 ; i < num_of_threads ; i++) {
            if (!threads[i]->busy) {
                if (pthread_create(&threads[i]->thread, NULL, (void *) requestHandle, (void *) &fd4)) {
                    printf("error");
                }
            }
        }

//	    requestHandle(connfd);
//	    Close(connfd);
    }

}
#pragma clang diagnostic pop


    


 
