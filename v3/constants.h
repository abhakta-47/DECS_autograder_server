#ifndef CONSTANTS_H
#define CONSTANTS_H

#define UUID_SIZE 37

#define STATUS_FOLDER "server_runtime/status/"
#define SUBMISSION_FOLDER "server_runtime/submissions/"
#define COMPILED_FOLDER "server_runtime/compiled/"
#define OUTPUT_FOLDER "server_runtime/outputs/"
#define DIFF_FOLDER "server_runtime/diff/"

#define ACTION_SIZE sizeof(int)
enum ACTIONS {
    ACTION_SUBMIT,
    ACTION_CHECK,
};

#define FLAG_SIZE sizeof(int)
enum FLAGS {
    CORRECT_ANSWER,
    WRONG_ANSWER,
    COMPILE_ERR,
    RUNTIME_ERR,
    OUTPUT_ERR,
};

#endif // CONSTANTS_H