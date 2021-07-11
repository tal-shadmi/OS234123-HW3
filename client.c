/*
 * client.c: A very, very primitive HTTP client.
 * 
 * To run, try: 
 *      ./client www.cs.technion.ac.il 80 /
 *
 * Sends one HTTP request to the specified HTTP server.
 * Prints out the HTTP response.
 *
 * HW3: For testing your server, you will want to modify this client.  
 * For example:
 * 
 * You may want to make this multi-threaded so that you can 
 * send many requests simultaneously to the server.
 *
 * You may also want to be able to request different URIs; 
 * you may want to get more URIs from the command line 
 * or read the list from a file. 
 *
 * When we test your server, we will be using modifications to this client.
 *
 */

#include "segel.h"

pthread_mutex_t client_respond_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * Send an HTTP request for the specified file 
 */
void clientSend(int fd, char *filename)
{
  char buf[MAXLINE];
  char hostname[MAXLINE];

  Gethostname(hostname, MAXLINE);

  /* Form and send the HTTP request */
  sprintf(buf, "GET %s HTTP/1.1\n", filename);
  sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
  Rio_writen(fd, buf, strlen(buf));
}
  
/*
 * Read the HTTP response and print it out
 */
void clientPrint(int fd)
{
  rio_t rio;
  char buf[MAXBUF];  
  int length = 0;
  int n;
  
  Rio_readinitb(&rio, fd);

  /* Read and display the HTTP Header */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (strcmp(buf, "\r\n") && (n > 0)) {
    printf("Header: %s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);

    /* If you want to look for certain HTTP tags... */
    if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
      printf("Length = %d\n", length);
    }
  }

  /* Read and display the HTTP Body */
  n = Rio_readlineb(&rio, buf, MAXBUF);
  while (n > 0) {
    printf("%s", buf);
    n = Rio_readlineb(&rio, buf, MAXBUF);
  }
}

typedef struct {
    char *host, *filename;
    int port;
} infoToThread;

void thread_client_routine(infoToThread *t) {
    int clientfd;
    for (int j = 0; j < 60; j++) {
        clientfd = Open_clientfd(t->host, t->port);
        if (j % 3 == 0) {
            clientSend(clientfd, t->filename);
        }
        else if (j % 3 == 1) {
            clientSend(clientfd, "/output.cgi");
        }
        else if (j % 3 == 2) {
            clientSend(clientfd, "/favicon.ico");
        }
        pthread_mutex_lock(&client_respond_mutex);
        clientPrint(clientfd);
        pthread_mutex_unlock(&client_respond_mutex);
        Close(clientfd);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <host> <port> <filename>\n", argv[0]);
        exit(1);
    }
    infoToThread *t = (infoToThread *) malloc(sizeof (infoToThread));
    t->host = strdup(argv[1]);
    t->port = atoi(argv[2]);
    t->filename = strdup(argv[3]);

    pthread_t threads[20];
    for (int i = 0 ; i < 20 ; i++) {
            if (pthread_create(&threads[i], NULL, (void *)thread_client_routine,(void *)t )) {
                printf("error");
            }
    }
    for (int i = 0 ; i < 20 ; i++) {
            if (pthread_join(threads[i], NULL)) {
                printf("error");
            }
    }
    exit(0);
    /* Open a single connection to the specified host and port */

}
