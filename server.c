/* Compile using: gcc -o server -pthread server.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h> // Include the pthread library
#include "myqueue.h"

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
  while (1) 
  {
        pthread_mutex_lock(&queue_mutex);
        while (pool_size == 0) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        int *clientSocket = dequeue();
        pool_size--;
        pthread_mutex_unlock(&queue_mutex);

        if (clientSocket) 
        {
          char buffer[1024];
              int n;

              bzero(buffer, 1024);
              int fileDescriptor = open("c_code_server.c", O_WRONLY | O_TRUNC | O_CREAT, 0666);
              n = read(*clientSocket, buffer, 1024);

              write(fileDescriptor, buffer, n);
              close(fileDescriptor);
              sleep(2);
              int stderrFileDescriptor = open("error_output.txt", O_WRONLY | O_CREAT, 0666);
              if (stderrFileDescriptor < 0)
                error("ERROR opening error_output.txt");

              // Redirect stderr to the file
              if (dup2(stderrFileDescriptor, STDERR_FILENO) == -1)
                error("ERROR redirecting stderr");

              // Close the original stderr file descriptor
              close(stderrFileDescriptor);

              if (n < 0)
                error("ERROR reading from socket");

              const char *compileCommand = "/usr/bin/gcc c_code_server.c -o c_code_server";
              int compileStatus = system(compileCommand);
              if (compileStatus == 0) { // If there are no compiler errors
                const char *runCommand = "./c_code_server > out_gen.txt";
                int runStatus = system(runCommand);

                if (runStatus != 0) { // If there are no runtime errors
                  const char *diffCommand = "/usr/bin/diff expected_output.txt out_gen.txt > diff_out.txt";
                  int diffStatus = system(diffCommand);

                  //printf("done\n");
                  fflush(stdout); // Flush the output buffer

                  if (diffStatus == 0) {
                    // If both files are the same according to diff, it returns 0
                    int outFileDescriptor = open("out_gen.txt", O_RDWR);
                    char outBuffer[1024];
                    int outBytesRead = read(outFileDescriptor, outBuffer, 1024);
                    lseek(fileDescriptor, 0, SEEK_SET);
                    ftruncate(fileDescriptor, 0);
                    write(outFileDescriptor, "\nPASS : ", strlen("\nPASS : "));
                    write(outFileDescriptor, outBuffer, outBytesRead);
                    close(outFileDescriptor);
                    outFileDescriptor = open("out_gen.txt", O_RDONLY);
                    outBytesRead = read(outFileDescriptor, outBuffer, 1024);
                    write(*clientSocket, outBuffer, outBytesRead);
                    close(outFileDescriptor);
                  } else {
                    // OUTPUT ERROR i.e., diff returns a non-zero value
                    int diffFileDescriptor = open("diff_out.txt", O_RDWR);
                    char outBuffer[1024];
                    int outBytesRead = read(diffFileDescriptor, outBuffer, 1024);
                    lseek(fileDescriptor, 0, SEEK_SET);
                    ftruncate(fileDescriptor, 0);
                    write(diffFileDescriptor, "\nOUTPUT ERROR :\n", strlen("\nOUTPUT ERROR :\n"));
                    write(diffFileDescriptor, outBuffer, outBytesRead);
                    close(diffFileDescriptor);
                    diffFileDescriptor = open("diff_out.txt", O_RDONLY);
                    outBytesRead = read(diffFileDescriptor, outBuffer, 1024);
                    write(*clientSocket, outBuffer, outBytesRead);
                    close(diffFileDescriptor);
                  }
                } else { // If there's a runtime error
                  int errorFileDescriptor = open("error_output.txt", O_RDWR);
                  char outBuffer[1024];
                  int outBytesRead = read(errorFileDescriptor, outBuffer, 1024);
                  lseek(fileDescriptor, 0, SEEK_SET);
                  ftruncate(fileDescriptor, 0);
                  write(errorFileDescriptor, "\nRUNTIME ERROR :\n ", strlen("\nRUNTIME ERROR :\n "));
                  write(errorFileDescriptor, outBuffer, outBytesRead);
                  close(errorFileDescriptor);
                  errorFileDescriptor = open("error_output.txt", O_RDONLY);
                  outBytesRead = read(errorFileDescriptor, outBuffer, 1024);
                  write(*clientSocket, outBuffer, outBytesRead);
                  close(errorFileDescriptor);
                }
              } else { // If there's a compilation error
                int errorFileDescriptor = open("error_output.txt", O_RDWR);
                char outBuffer[1024];
                int outBytesRead = read(errorFileDescriptor, outBuffer, 1024);
                lseek(fileDescriptor, 0, SEEK_SET);
                ftruncate(fileDescriptor, 0);
                write(errorFileDescriptor, "\nCOMPILATION ERROR :\n ", strlen("\nCOMPILATION ERROR :\n "));
                write(errorFileDescriptor, outBuffer, outBytesRead);
                close(errorFileDescriptor);
                errorFileDescriptor = open("error_output.txt", O_RDONLY);
                outBytesRead = read(errorFileDescriptor, outBuffer, 1024);
                write(*clientSocket, outBuffer, outBytesRead);
                close(errorFileDescriptor);
              }
                      
                close(*clientSocket);
                free(clientSocket);
        }
  }

}

int main(int argc, char *argv[]) {
  int listenSocket, portNumber;
  socklen_t clientAddressLength;
  struct sockaddr_in serverAddress, clientAddress;
  //pthread_t thread_id; // Thread ID for creating worker threads

  pthread_t thread_pool[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&thread_pool[i], NULL, handle_client, NULL);
    }


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

  if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    error("ERROR on binding");

  listen(listenSocket, 30);

  clientAddressLength = sizeof(clientAddress);

  while (1) {
    int *newSocket = (int *)malloc(sizeof(int));
    *newSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);

    if (*newSocket < 0)
      error("ERROR on accept");


    pthread_mutex_lock(&queue_mutex);
    enqueue(newSocket);
    pool_size++;
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
    // Create a new thread to handle the client request
    // if (pthread_create(&thread_id, NULL, handle_client, (void *)newSocket) != 0) {
    //   perror("pthread_create");
    //   exit(1);
    // }

    
    // Detach the thread to avoid memory leaks
    //pthread_detach(thread_id);
  }
  for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thread_pool[i], NULL);
    }
  close(listenSocket);

  return 0;
}
