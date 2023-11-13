#ifndef COMMON_H
#define COMMON_H

#define BUFFER_SIZE 1024
#define MAX_FILE_SIZE_BYTES 4

enum TextColor { Default, Red, Green, Blue, Yellow, Cyan, Magenta, White };

char *colorizeText(const char *text, enum TextColor color);

int sock_read_int(int sock, int size);
int sock_write_int(int sock, int value, int size);
int recv_file(int sockfd, const char *file_path);
int send_file(int sockfd, const char *file_path);

#endif // COMMON_H
