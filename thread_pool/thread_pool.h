#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <stdbool.h>

#define QUEUE_SIZE 100

// Define a structure for a task
typedef struct {
    void (*function)(void *); // Function to execute
    void *argument;           // Argument for the function
} Task;

// Define the ThreadPool structure
struct ThreadPool {
    int num_threads;          // Number of worker threads
    int queue_size;           // Size of the task queue
    pthread_t *threads;       // Array of worker threads
    Task *task_queue;         // Task queue
    int front, rear;          // Task queue pointers
    pthread_mutex_t lock;     // Mutex for synchronization
    pthread_cond_t not_empty; // Condition variable for task availability
    bool shutdown;            // Flag to signal thread pool shutdown
};

// Define a structure for the thread pool
typedef struct ThreadPool ThreadPool;

// Initialize the thread pool
ThreadPool *ThreadPoolInit(int num_threads);

// Add a task to the thread pool
void ThreadPoolAddTask(ThreadPool *pool, void (*function)(void *),
                       void *argument);

// Wait for all tasks to complete
void ThreadPoolWait(ThreadPool *pool);

// Clean up the thread pool
void ThreadPoolCleanup(ThreadPool *pool);

#endif // THREADPOOL_H
