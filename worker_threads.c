#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "constants.h"

// Function to run command with system call
int run_command(const char *command) {
    printf("\nRunning command : %s\n", command);
    int status = system(command);
    printf("Command status : %d\n\n", status);
    return status;
}

// Function to compile the code
int compile_task(const char *req_id) {
    // "/usr/bin/gcc c_code_server.c -o c_code_server";
    printf("Compile task for %s started\n", req_id);
    const char *complieCommands[] = {"/usr/bin/gcc ",
                                     SUBMISSION_FOLDER,
                                     req_id,
                                     ".c -o ",
                                     COMPILED_FOLDER,
                                     req_id,
                                     NULL};
    const char *compileCommand = concat_strings(complieCommands, "");
    int compileStatus = run_command(compileCommand);
    printf("Compile task for %s ended\n", req_id);
    return compileStatus;
}

// Function to run the compiled code
int run_task(const char *req_id) {
    printf("Run task for %s started\n", req_id);
    // "./c_code_server > out_gen.txt";
    const char *runCommands[] = {COMPILED_FOLDER, req_id, " > ",
                                 OUTPUT_FOLDER,   req_id, NULL};
    const char *runCommand = concat_strings(runCommands, "");
    int runStatus = run_command(runCommand);
    printf("Run task for %s ended\n", req_id);
    return runStatus;
}

// Function to compare the generated output with expected output
int diff_task(const char *req_id) {
    printf("Diff task for %s started\n", req_id);
    // "/usr/bin/diff expected_output.txt out_gen.txt > diff_out.txt";
    const char *diffCommands[] = {"/usr/bin/diff ",
                                  "expected_output.txt ",
                                  OUTPUT_FOLDER,
                                  req_id,
                                  " > ",
                                  DIFF_FOLDER,
                                  req_id,
                                  NULL};
    const char *diffCommand = concat_strings(diffCommands, "");
    int diffStatus = run_command(diffCommand);
    printf("Diff task for %s ended\n", req_id);
    return diffStatus;
}

// Worker task
int worker_task(const char *req_id) {
    printf("Worker task for %s started\n", req_id);
    char status_file_path[UUID_SIZE + strlen(STATUS_FOLDER)];
    strcpy(status_file_path, STATUS_FOLDER);
    strcat(status_file_path, req_id);
    FILE *status_fd = fopen(status_file_path, "w");
    int compileStatus = compile_task(req_id);
    if (compileStatus != 0) {
        fprintf(status_fd, "COMPILE_ERROR");
        return 0;
    }
    fprintf(status_fd, "COMPILED");

    int runStatus = run_task(req_id);
    if (runStatus != 0) {
        fprintf(status_fd, "RUNTIME_ERROR");
        return 0;
    }
    fprintf(status_fd, "EXECUTED");

    int diffStatus = diff_task(req_id);
    if (diffStatus != 0) {
        fprintf(status_fd, "SUCCESS");
        return 0;
    } else {
        fprintf(status_fd, "WRONG_ANSWER");
        return 0;
    }

    fclose(status_fd);
}
