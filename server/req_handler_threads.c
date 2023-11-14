#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/common.h"
#include "common/gen_uuid.h"
#include "constants.h"
#include "req_handler_threads.h"
#include "thread_pool/thread_pool.h"
#include "worker_threads.h"

extern struct ThreadPool *global_thread_pool;

int file_exists(char *file_paht) {
    // todo implement function
}

int req_handler(int *clientSocket) {
    // Done : read only flag portion of socket file send socket to
    // new_submission or status_check
    int sockfd = *clientSocket;
    int action = sock_read_int(sockfd);
    switch (action) {
    case ACTION_SUBMIT:
        // Todo handle error ie returns -1
        new_submission(sockfd);
        break;
    case ACTION_CHECK:
        // Todo handle error ie returns -1
        status_check(sockfd);
        break;
    default:
        perror("Invalid action");
    }
    close(sockfd);
}

int new_submission(int sockfd) {
    // Done : gen uuid save payload file to appropriate location
    char *uuid = gen_uuid();

    printf("UUID : %s\n", uuid);
    char cpp_file_path[UUID_SIZE + strlen(SUBMISSION_FOLDER) + 2];
    strcpy(cpp_file_path, SUBMISSION_FOLDER);
    strcat(cpp_file_path, uuid);
    strcat(cpp_file_path, ".c");
    if (recv_file(sockfd, cpp_file_path) == -1) {
        perror("Error receiving file");
        return -1;
    }

    const char *status_file_path[] = {STATUS_FOLDER, uuid, NULL};
    char *status_file = concat_strings(status_file_path, "");
    FILE *status_fd = fopen(status_file, "w");
    fprintf(status_fd, "RECEIVED");
    fclose(status_fd);
    free(status_file);

    struct ThreadPool *thread_pool = global_thread_pool;
    ThreadPoolAddTask(thread_pool, worker_task, (void *)uuid);

    return write(sockfd, uuid, UUID_SIZE);
}

int status_check(int sockfd) {
    // Done : get uuid from socket, read appropriate status file a return
    char req_id[UUID_SIZE];
    if (recv(sockfd, req_id, UUID_SIZE, 0) == -1) {
        return -1;
    }
    char status_file_path[UUID_SIZE + strlen(STATUS_FOLDER)];
    strcpy(status_file_path, STATUS_FOLDER);
    strcat(status_file_path, req_id);
    // TODO file doesn't exist
    return send_file(sockfd, status_file_path);
}