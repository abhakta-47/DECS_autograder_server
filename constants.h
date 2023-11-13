#ifndef CONSTANTS_H
#define CONSTANTS_H

#define UUID_SIZE 37

#define STATUS_FOLDER "status/"
#define SUBMISSION_FOLDER "submissions/"
#define COMPILED_FOLDER "compiled/"
#define OUTPUT_FOLDER "outputs/"
#define DIFF_FOLDER "diff/"

#define ACTION_SIZE sizeof(int)
enum ACTIONS {
    ACTION_SUBMIT,
    ACTION_CHECK,
};

#define FLAG_SIZE sizeof(int)
enum FLAGS {
    SUCCESS,
    COMPILE_ERR,
    RUNTIME_ERR,
    OUTPUT_ERR,
};

#endif // CONSTANTS_H