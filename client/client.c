#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/common.h"
#include "constants.h"

void error(char *msg) {
    perror(msg);
    exit(1);
}

double totalResponseTime = 0.0;
int successfulRes = 0;
int numOfTimeouts = 0;
int numErrResponses = 0;

struct timeval timeout;

int check_func(int sockfd, char *req_id) {
    sock_write_int(sockfd, ACTION_CHECK);
    send(sockfd, req_id, strlen(req_id), 0);
    char server_response[1024];
    bzero(server_response, sizeof(server_response));
    int server_response_len =
        read(sockfd, server_response, sizeof(server_response));

    if (server_response_len < 0) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            numOfTimeouts++;
            printf("timeout detected");
        } else {
            numErrResponses++;
            return -1;
        }
    }

    printf("server : %s", server_response);

    if (strcmp(server_response, "SUCCESS"))
        return 0;
    if (strcmp(server_response, "COMPILE_ERROR"))
        return 1;
    if (strcmp(server_response, "RUNTIME_ERROR"))
        return 2;
    if (strcmp(server_response, "WRONG_ANSWER"))
        return 3;
    if (strcmp(server_response, "RECEIVED"))
        return 4;

    return 5;
}

int submit_func(int sockfd, char *NameOfFile, char *req_id) {

    sock_write_int(sockfd, ACTION_SUBMIT);
    send_file(sockfd, NameOfFile);

    // Code to handle server response and other operations
    char server_to_client_response_file[] =
        "server_to_client_response_file.txt";
    int server_to_client_response_file_fd =
        open(server_to_client_response_file, O_CREAT | O_RDWR | O_TRUNC, 0777);

    char server_response[1024];
    bzero(server_response, sizeof(server_response));
    int server_response_len =
        read(sockfd, server_response, sizeof(server_response));

    if (server_response_len < 0) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            numOfTimeouts++;
            printf("timeout detected");
        } else {
            numErrResponses++;
            return -1;
        }
    }

    write(server_to_client_response_file_fd, server_response,
          server_response_len);
    printf("%s", server_response);
    strcpy(req_id, server_response);
}

int new_sockfd(char *serverAddressVar, int portno) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(serverAddressVar);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                   sizeof(timeout)) < 0)
        error("setsockopt (SO_RCVTIMEO) failed\n");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr,
                "Usage: %s localhost <server_port> <sourceCodeFileTobeGraded> "
                "<loop> <sleepTimeSeconds> <timeout-seconds>\n",
                argv[0]);
        exit(1);
    }

    char *serverAddressVar = argv[1];
    int portno = atoi(argv[2]);
    char *sourceCodeFileVar = argv[3];
    int loop = atoi(argv[4]);
    int sleepTime = atoi(argv[5]);
    int timeout_seconds = atoi(argv[6]);
    timeout.tv_sec = timeout_seconds;

    // Send source file to server
    char *NameOfFile = sourceCodeFileVar;

    int bytesRead;
    struct timeval start, end;
    char req_id[37];
    int sockfd;

    struct timeval loop_start, loop_end;

    double totalResponseTime = 0.0;
    gettimeofday(&loop_start, NULL);
    for (int i = 0; i < loop; i++) {
        double responseTime = 0.0;
        printf("loop: %d\n", i);
        sockfd = new_sockfd(serverAddressVar, portno);
        gettimeofday(&start, NULL);
        submit_func(sockfd, NameOfFile, req_id);
        gettimeofday(&end, NULL);
        responseTime += (double)(end.tv_sec - start.tv_sec) +
                        (double)(end.tv_usec - start.tv_usec) / 1000000.0;
        printf("req_id: %s\n", req_id);
        sleep(sleepTime);

        int status = 4, status_check_counter =0 ;
        while (status == 4 && status_check_counter++ <= 10) {
            int sockfd = new_sockfd(serverAddressVar, portno);
            gettimeofday(&start, NULL);
            status = check_func(sockfd, req_id);
            gettimeofday(&end, NULL);
            close(sockfd);
            sleep(sleepTime);
            responseTime += (double)(end.tv_sec - start.tv_sec) +
                            (double)(end.tv_usec - start.tv_usec) / 1000000.0;
        }
        if (status != 5 && status_check_counter <= 10)
            successfulRes++;
        totalResponseTime += responseTime;

        sleep(sleepTime);
    }
    gettimeofday(&loop_end, NULL);

    double loopTime =
        (double)(loop_end.tv_sec - loop_start.tv_sec) +
        (double)(loop_end.tv_usec - loop_start.tv_usec) / 1000000.0;

    // Calculate average response time
    double averageResponseTime = totalResponseTime / successfulRes;
    double throughputValue = successfulRes / (1.0 * totalResponseTime);
    double throughputValue_loop = successfulRes / (1.0 * loopTime);
    // Output results
    printf("Throughput: %.7f\n", throughputValue);
    printf("Throughput looptime: %.7f\n", throughputValue_loop);
    printf("Average Response Time: %.7f\n", averageResponseTime);
    printf("Success Rate: %.7f\n", (double)successfulRes / (loop * 1.0) * 100);

    return 0;
}
