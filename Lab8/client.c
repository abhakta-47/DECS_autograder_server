#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  if (argc != 7) {
    fprintf(stderr, "Usage: %s localhost <server_port> <sourceCodeFileTobeGraded> <loop> <sleepTimeSeconds> <timeout>\n", argv[0]);
    exit(1);
  }

  double totalResponseTime = 0.0;
  int successfulRes = 0;
  int errorRes = 0;
  int num_timeout = 0;

  char *serverAddressVar = argv[1];
  int portno = atoi(argv[2]);
  char *sourceCodeFileVar = argv[3];
  int loop = atoi(argv[4]);
  int sleepTime = atoi(argv[5]);
  int timeout_val = atoi(argv[6]);

  int sockfd, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[1024];

  for (int i = 0; i < loop; i++) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
      error("ERROR opening socket");

    server = gethostbyname(serverAddressVar);

    if (server == NULL) {
      fprintf(stderr, "ERROR, no such host\n");
      exit(1);
    }

    struct timeval timeout;
    timeout.tv_sec = timeout_val;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
      num_timeout++;
      error("setsockopt (SO_RCVTIMEO) failed\n");
    }




    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
      num_timeout++;
      error("ERROR connecting");
    }
    // send source file to server
    char *NameOfFile = sourceCodeFileVar;
    int SourceOfFile = open(NameOfFile, O_RDONLY);

    if (SourceOfFile < 0) {
      errorRes++;
      error("Can't open the source file");
    }

    char SourceOfFileBuf[1024];
    int bytesRead;
    struct timeval start, end;

    gettimeofday(&start, NULL);

    while ((bytesRead = read(SourceOfFile, SourceOfFileBuf, sizeof(SourceOfFileBuf))) > 0) {
      write(sockfd, SourceOfFileBuf, bytesRead);
    }

  char *server_to_client_response_file = "server_to_client_response_file.txt";
  int server_to_client_response_file_fd = open(server_to_client_response_file, O_CREAT | O_RDWR | O_TRUNC, 0777);
  
  char server_response[256];
  bzero(server_response, sizeof(server_response));
  int server_response_len = read(sockfd, server_response, sizeof(server_response));
  write(server_to_client_response_file_fd, server_response, server_response_len);


    gettimeofday(&end, NULL);
    double responseTime = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
    totalResponseTime += responseTime;
    successfulRes++;  

    sleep(sleepTime);

    close(SourceOfFile);
    close(sockfd);
  }

  double averageResponseTime = totalResponseTime / successfulRes;
  double throughputValue =successfulRes/(1.0*totalResponseTime); 
  double successful_request_rate = successfulRes / totalResponseTime;
  double timeout_rate = num_timeout / totalResponseTime;
  double error_rate = errorRes / totalResponseTime;
  double request_rate = loop / totalResponseTime;


  printf("Throughput is : %.7f\n", throughputValue);
  printf("Average Response Time is : %.7f\n", averageResponseTime);
  printf("Successful Request Rate is : %.7f\n", successful_request_rate);
  printf("Total Response Time is : %.7f\n", totalResponseTime);
  printf("Timeout Rate is : %.7f\n", timeout_rate);
  printf("Error rate is : %.7f\n", error_rate);
  printf("Request rate is : %.7f\n", request_rate);

  //printf("Average Response Time: %.7f\n successfulRes: %d\n totalResponse Time: %.7f\n throughput: %.7f", averageResponseTime,successfulRes,totalResponseTime, throughputValue);

  return 0;
}

