/* Compile using: gcc -o server -pthread server.c */

#include "myqueue.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h> // Include the pthread library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/common.h"
#include "logger/logger.h"

int MAX_THREADS = 10; // Maximum number of threads in the pool

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
int pool_size;

void error(char *msg) {
    perror(msg);
    exit(1);
}

// Function to run command with system call
int run_command(const char *command) {
    int status = system(command);
    return status;
}

// Function to compile the code
int compile_task(const char *thread_id) {
    // "/usr/bin/gcc c_code_server.c -o c_code_server";
    log_message(LOG_INFO, "worker: compiler started");
    const char *complieCommands[] = {
        "/usr/bin/gcc ", thread_id, "_submission.c -o ", thread_id, ".o", NULL};
    const char *compileCommand = concat_strings(complieCommands, "");
    int compileStatus = run_command(compileCommand);
    log_message(LOG_INFO, "worker: compiler ended: %d", compileStatus);
    return compileStatus;
}

// Function to run the compiled code
int run_task(const char *thread_id) {
    //  bash -c './140479336359616.o; if [ $? -eq 0 ]; then true; else false;
    //  fi' >> out.txt
    log_message(LOG_INFO, "worker: runner started");
    // const char *runCommands[] = {"bash -c ./", thread_id, ".o ", " &> ",
    //  thread_id,    ".out",    NULL};
    // const char *runCommand = concat_strings(runCommands, "");
    const char runCommand[200];
    sprintf(runCommand,
            "bash -c './%s.o; if [ $? -eq 0 ]; then true; else "
            "false; fi' &> %s.out",
            thread_id, thread_id);
    log_message(LOG_INFO, "worker: runner command: %s", runCommand);
    int runStatus = run_command(runCommand);
    log_message(LOG_INFO, "worker: runner ended: %d", runStatus);
    return runStatus;
}

// Function to compare the generated output with expected output
int diff_task(const char *thread_id) {
    // "/usr/bin/diff expected_output.txt out_gen.txt > diff_out.txt";
    log_message(LOG_INFO, "worker: diff started");
    const char *diffCommands[] = {"/usr/bin/diff ",
                                  "expected_output.txt ",
                                  thread_id,
                                  ".out",
                                  " > ",
                                  thread_id,
                                  ".diff",
                                  NULL};
    const char *diffCommand = concat_strings(diffCommands, "");
    int diffStatus = run_command(diffCommand);
    log_message(LOG_INFO, "worker: diff ended: %d", diffStatus);
    return diffStatus;
}

// Worker task
void *handle_client(int *clientSocketFD) {
    char buffer[1024];
    int n;
    int clientSocket = *clientSocketFD;

    pthread_t thread_id = pthread_self();
    char thread_str[20];
    snprintf(thread_str, sizeof(thread_str), "%lu", (unsigned long)thread_id);

    bzero(buffer, 1024);
    const char *file_name[] = {thread_str, "_submission.c", NULL};
    const char *file_name_str = concat_strings(file_name, "");
    recv_file(clientSocket, file_name_str);

    int compileStatus = compile_task(thread_str);
    if (compileStatus != 0) {
        write(clientSocket, "COMPILE_ERROR", strlen("COMPILE_ERROR"));
        return;
    }
    int runStatus = run_task(thread_str);
    if (runStatus != 0) {
        write(clientSocket, "RUNTIME_ERROR", strlen("RUNTIME_ERROR"));
        return;
    }

    int diffStatus = diff_task(thread_str);

    if (diffStatus != 0)
        log_message(LOG_INFO, "worker: Wrong answer");
    else
        log_message(LOG_INFO, "worker: Accepted");

    log_message(LOG_INFO, "worker: network: sending result");
    if (diffStatus != 0)
        write(clientSocket, "WRONG_ANSWER", strlen("WRONG_ANSWER"));
    else
        write(clientSocket, "ACCEPTED", strlen("ACCEPTED"));
    log_message(LOG_INFO, "worker: network: result sent");

    close(clientSocket);
    log_message(LOG_INFO, "worker: client socket closed");
}

void *worker_function(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (pool_size == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        int *clientSocket = dequeue();
        log_message(LOG_INFO, "client dequeued");
        pool_size--;
        pthread_mutex_unlock(&queue_mutex);

        log_message(LOG_INFO, "worker: handling client");
        handle_client(clientSocket);
        log_message(LOG_INFO, "worker: client handled\n");
    }
}

int main(int argc, char *argv[]) {
    int listenSocket, portNumber;
    socklen_t clientAddressLength;
    struct sockaddr_in serverAddress, clientAddress;

    if (argc < 3) {
        fprintf(stderr, "USAGE: %s port_number number_of_threads\n", argv[0]);
        exit(1);
    }

    MAX_THREADS = atoi(argv[2]);
    log_message(LOG_INFO, "Server started %d\n", MAX_THREADS);
    pthread_t thread_pool[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&thread_pool[i], NULL, worker_function, NULL);
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

        log_message(LOG_INFO, "New client connected");
        if (*newSocket < 0)
            error("ERROR on accept");

        pthread_mutex_lock(&queue_mutex);
        enqueue(newSocket);
        pool_size++;
        log_message(LOG_INFO, "client enqueued & signalling threads");
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&queue_mutex);
    }
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread_pool[i], NULL);
    }
    close(listenSocket);

    return 0;
}
