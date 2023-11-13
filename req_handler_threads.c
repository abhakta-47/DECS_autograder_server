#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"
#include "constants.h"
#include "gen_uuid.h"
#include "req_handler_threads.h"

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
    char cpp_file_path_size[UUID_SIZE + strlen(SUBMISSION_FOLDER)];
    strcpy(cpp_file_path_size, SUBMISSION_FOLDER);
    strcat(cpp_file_path_size, uuid);
    recv_file(sockfd, cpp_file_path_size);
    // return "id";
    write(sockfd, uuid, UUID_SIZE);
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