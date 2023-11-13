/* Compile using: gcc -o server -pthread server.c */

#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h> // Include the pthread library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "myqueue.h"
#include "req_handler_threads.h"
#include "thread_pool.h"

#define MAX_THREADS 20 // Maximum number of threads in the pool

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
int pool_size;

void error(char *msg) {
    perror(msg);
    exit(1);
}

// Function to handle each client request in a separate thread
void *handle_client(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (pool_size == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        int *clientSocket = dequeue();
        pool_size--;
        pthread_mutex_unlock(&queue_mutex);

        if (clientSocket) {
            req_handler(clientSocket);
            close(*clientSocket);
            free(clientSocket);
        }
    }
}

int main(int argc, char *argv[]) {
    int listenSocket, portNumber;
    socklen_t clientAddressLength;
    struct sockaddr_in serverAddress, clientAddress;
    // pthread_t thread_id; // Thread ID for creating worker threads

    // pthread_t thread_pool[MAX_THREADS];
    // for (int i = 0; i < MAX_THREADS; i++) {
    //     pthread_create(&thread_pool[i], NULL, handle_client, NULL);
    // }
    struct ThreadPool *req_pool = ThreadPoolInit(MAX_THREADS);

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket < 0)
        error("ERROR opening socket");

    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    portNumber = atoi(argv[1]);
    serverAddress.sin_port = htons(portNumber);

    if (bind(listenSocket, (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) < 0)
        error("ERROR on binding");

    listen(listenSocket, 30);

    clientAddressLength = sizeof(clientAddress);

    while (1) {
        int *newSocket = (int *)malloc(sizeof(int));
        *newSocket = accept(listenSocket, (struct sockaddr *)&clientAddress,
                            &clientAddressLength);

        if (*newSocket < 0)
            error("ERROR on accept");

        ThreadPoolAddTask(req_pool, req_handler, (void *)newSocket);

        // pthread_mutex_lock(&queue_mutex);
        // enqueue(newSocket);
        // pool_size++;
        // pthread_cond_signal(&queue_cond);
        // pthread_mutex_unlock(&queue_mutex);
        // Create a new thread to handle the client request
        // if (pthread_create(&thread_id, NULL, handle_client, (void
        // *)newSocket) != 0) {
        //   perror("pthread_create");
        //   exit(1);
        // }

        // Detach the thread to avoid memory leaks
        // pthread_detach(thread_id);
    }
    // for (int i = 0; i < MAX_THREADS; i++) {
    //     pthread_join(thread_pool[i], NULL);
    // }
    close(listenSocket);

    return 0;
}
