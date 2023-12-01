#ifndef COMMON_H
#define COMMON_H

#define BUFFER_SIZE 1024
#define INT_SOCK_BUFFER_SIZE 4

enum TextColor { Default, Red, Green, Blue, Yellow, Cyan, Magenta, White };

char *colorizeText(const char *text, enum TextColor color);

int sock_read_int(int sock);
int sock_write_int(int sock, int value);
int recv_file(int sockfd, const char *file_path);
int send_file(int sockfd, const char *file_path);
char *concat_strings(const char *[], const char *);

#endif // COMMON_H
