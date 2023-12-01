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

#include "common/myqueue.h"
#include "req_handler_threads.h"
#include "thread_pool/thread_pool.h"

#define MAX_THREADS 20 // Maximum number of threads in the pool

void error(char *msg) {
    perror(msg);
    exit(1);
}

extern struct ThreadPool *global_thread_pool = NULL;

int main(int argc, char *argv[]) {
    int listenSocket, portNumber;
    socklen_t clientAddressLength;
    struct sockaddr_in serverAddress, clientAddress;

    if (argc < 3) {
        fprintf(stderr, "Usage, ./server <port> <threadpool size>\n");
        exit(1);
    }

    int threadpool_size = atoi(argv[2]);
    struct ThreadPool *thread_pool;
    thread_pool = ThreadPoolInit(threadpool_size);
    global_thread_pool = thread_pool;

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
        log_message(LOG_INFO, "server: Connection accepted");
        if (*newSocket < 0)
            error("ERROR on accept");

        ThreadPoolAddTask(thread_pool, req_handler, (void *)newSocket);
    }

    close(listenSocket);
    ThreadPoolWait(thread_pool);
    ThreadPoolCleanup(thread_pool);

    return 0;
}
