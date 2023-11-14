#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

int sock_read_int(int sock) {
    int val;
    if (recv(sock, &val, sizeof(val), 0) == -1) {
        perror("Error receiving file size");
        return -1;
    }
    return val;
}

int sock_write_int(int sock, int value) {
    if (send(sock, &value, sizeof(value), 0) == -1) {
        perror("Error sending file size");
        return -1;
    }
}

char *colorizeText(const char *text, enum TextColor color) {
    const char *escapeCode = "\033[";

    // Define color codes
    const char *colorCodes[] = {
        "0m",    // Default color
        "1;31m", // Red
        "1;32m", // Green
        "1;34m", // Blue
        "1;33m", // Yellow
        "1;36m", // Cyan
        "1;35m", // Magenta
        "1;37m"  // White
    };

    char *colorizedText = (char *)malloc(
        strlen(escapeCode) + strlen(colorCodes[color]) + strlen(text) +
        strlen(escapeCode) + strlen(colorCodes[0]) + 1);

    strcpy(colorizedText, escapeCode);
    strcat(colorizedText, colorCodes[color]);
    strcat(colorizedText, text);
    strcat(colorizedText, escapeCode);
    strcat(colorizedText, colorCodes[0]);

    return colorizedText;
}

int recv_file(int sockfd, const char *file_path) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    FILE *file = fopen(file_path, "wb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int file_size;
    file_size = sock_read_int(sockfd);
    if (file_size == -1) {
        perror("Error receiving file size");
        return -1;
    }

    size_t bytes_read = 0, total_bytes_read = 0;
    while (total_bytes_read < file_size) {
        bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
        total_bytes_read += bytes_read;

        if (bytes_read <= 0) {
            perror("Error receiving file data");
            fclose(file);
            return -1;
        }

        fwrite(buffer, 1, bytes_read, file);
        memset(buffer, 0, BUFFER_SIZE);
    }
    fclose(file);
    return 0;
}

int send_file(int sockfd, const char *file_path) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if (sock_write_int(sockfd, file_size) == -1) {
        perror("Error sending file size");
        fclose(file);
        return -1;
    }

    while (!feof(file)) {
        size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);

        if (send(sockfd, buffer, bytes_read, 0) == -1) {
            perror("Error sending file data");
            fclose(file);
            return -1;
        }

        memset(buffer, 0, BUFFER_SIZE);
    }

    fclose(file);
    return 0;
}

// given array of char* ending with NULL,
// returns a string with all the strings concatenated
char *concat_strings(const char *strings[], const char *separator) {
    int count = 0;

    size_t total_length = 0;

    while (strings[count] != NULL) {
        total_length += strlen(strings[count]);
        count++;
    }

    total_length += (count - 1) * strlen(separator);

    // Allocate memory for the concatenated string
    char *result =
        (char *)malloc(total_length + 1); // +1 for the null terminator

    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    // Concatenate the strings into the result
    strcpy(result, "");

    for (int i = 0; i < count; i++) {
        if (strings[i] != NULL) {
            strcat(result, strings[i]);
            strcat(result, separator);
        }
    }

    return result;
}
