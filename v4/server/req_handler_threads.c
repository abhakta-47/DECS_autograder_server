#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common/common.h"
#include "common/gen_uuid.h"
#include "constants.h"
#include "logger/logger.h"
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
    log_message(LOG_INFO, "req_handler: new Action : %d", action);
    switch (action) {
    case ACTION_SUBMIT:
        new_submission(sockfd);
        log_message(LOG_INFO, "req_handler: new_submission finished", sockfd);
        break;
    case ACTION_CHECK:
        status_check(sockfd);
        log_message(LOG_INFO, "req_handler: status_check finished", sockfd);
        break;
    default:
        perror("Invalid action");
    }
    close(sockfd);
}

int new_submission(int sockfd) {
    // Done : gen uuid save payload file to appropriate location
    char *uuid = gen_uuid();

    log_message(LOG_INFO, "new_submission: UUID generated: %s", uuid);
    char cpp_file_path[UUID_SIZE + strlen(SUBMISSION_FOLDER) + 2];
    strcpy(cpp_file_path, SUBMISSION_FOLDER);
    strcat(cpp_file_path, uuid);
    strcat(cpp_file_path, ".c");
    if (recv_file(sockfd, cpp_file_path) == -1) {
        log_message(LOG_ERROR, "new_submission: Error receiving file");
        perror("Error receiving file");
        return -1;
    }
    log_message(LOG_INFO, "new_submission: %s saved", uuid);

    const char *status_file_path[] = {STATUS_FOLDER, uuid, NULL};
    char *status_file = concat_strings(status_file_path, "");
    FILE *status_fd = fopen(status_file, "w");
    fprintf(status_fd, "RECEIVED");
    fclose(status_fd);
    free(status_file);
    log_message(LOG_INFO, "new_submission: %s Status file created", uuid);

    struct ThreadPool *thread_pool = global_thread_pool;
    ThreadPoolAddTask(thread_pool, worker_task, (void *)uuid);
    log_message(LOG_INFO, "new_submission: %s Worker task added", uuid);

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

    FILE *status_file = fopen(status_file_path, "r");

    if (status_file == NULL) {
        perror("Error opening status file");
        log_message(LOG_ERROR, "status_check %s: Error opening status file",
                    req_id);
        char *err_msg = "WRONG ID";
        write(sockfd, err_msg, strlen(err_msg));
        return -1;
    }

    //  get last line of status_file
    char status[1024];
    char *last_line = NULL;
    while (fgets(status, 1024, status_file) != NULL) {
        last_line = status;
    }

    log_message(LOG_INFO, "status_check %s: successfully read status file ",
                req_id);
    fclose(status_file);
    if (last_line == NULL) {
        char *err_msg = "WRONG ID";
        write(sockfd, err_msg, strlen(err_msg));
        return -1;
    }
    write(sockfd, last_line, strlen(last_line));
    return 0;
}