#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "constants.h"

char *concat_strings(const char *strings[]) {
    int count = 0;

    size_t total_length = 0;

    while (strings[count] != NULL) {
        total_length += strlen(strings[count]);
        count++;
    }

    total_length += count - 1;

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
            strcat(result, " ");
        }
    }

    return result;
}

// Function to compile the code
int compile_task(const char *req_id) {
    // "/usr/bin/gcc c_code_server.c -o c_code_server";
    const char *complieCommands[] = {"/usr/bin/gcc",
                                     SUBMISSION_FOLDER,
                                     req_id,
                                     "-o",
                                     COMPILED_FOLDER,
                                     req_id,
                                     NULL};
    const char *compileCommand = concat_strings(complieCommands);
    int compileStatus = system(compileCommand);
    return compileStatus;
}

// Function to run the compiled code
int run_task(const char *req_id) {
    // "./c_code_server > out_gen.txt";
    const char *runCommands[] = {COMPILED_FOLDER, req_id, ">",
                                 OUTPUT_FOLDER,   req_id, NULL};
    const char *runCommand = concat_strings(runCommands);
    int runStatus = system(runCommand);
    return runStatus;
}

// Function to compare the generated output with expected output
int diff_task(const char *req_id) {
    // "/usr/bin/diff expected_output.txt out_gen.txt > diff_out.txt";
    const char *diffCommands = {"/usr/bin/diff",
                                "expected_output.txt",
                                OUTPUT_FOLDER,
                                req_id,
                                ">",
                                DIFF_FOLDER,
                                req_id,
                                NULL};
    const char *diffCommand = concat_strings(diffCommands);
    int diffStatus = system(diffCommand);
    return diffStatus;
}

// Worker task
int worker_task(const char *req_id) {
    FILE *status_fd = fopen("status.txt", "w");
    int compileStatus = compile_task(req_id);
    if (compileStatus == 0) {
        fprintf(status_fd, "COMPILE_ERROR");
        return 0;
    }

    int runStatus = run_task(req_id);
    if (runStatus == 0) {
        fprintf(status_fd, "RUNTIME_ERROR");
        return 0;
    }

    int diffStatus = diff_task(req_id);
    if (diffStatus == 0) {
        fprintf(status_fd, "SUCCESS");
        return 0;
    } else {
        fprintf(status_fd, "WRONG_ANSWER");
        return 0;
    }

    fclose(status_fd);
}
