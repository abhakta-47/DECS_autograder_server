#include "thread_pool.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Function to execute tasks
void *ThreadWorker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (true) {
        pthread_mutex_lock(&(pool->lock));
        while (pool->front == pool->rear && !pool->shutdown) {
            // Wait for tasks to be available
            pthread_cond_wait(&(pool->not_empty), &(pool->lock));
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&(pool->lock));
            pthread_exit(NULL);
        }

        // Get the next task from the queue
        Task task = pool->task_queue[pool->front];
        pool->front = (pool->front + 1) % pool->queue_size;

        pthread_mutex_unlock(&(pool->lock));

        // Execute the task
        task.function(task.argument);
    }
    pthread_exit(NULL);
}

// Initialize the thread pool with a specified number of threads
ThreadPool *ThreadPoolInit(int num_threads) {
    log_message(LOG_INFO, "Initializing thread pool with %d threads",
                num_threads);
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
    if (pool == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    pool->num_threads = num_threads;
    pool->queue_size = QUEUE_SIZE;
    pool->threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    pool->task_queue = (Task *)malloc(QUEUE_SIZE * sizeof(Task));
    pool->front = pool->rear = 0;
    pool->shutdown = false;

    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->not_empty), NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&(pool->threads[i]), NULL, ThreadWorker, (void *)pool);
    }

    log_message(LOG_INFO, "Thread pool initialized");
    return pool;
}

// Add a task to the thread pool
void ThreadPoolAddTask(ThreadPool *pool, void (*function)(void *),
                       void *argument) {
    pthread_mutex_lock(&(pool->lock));

    while ((pool->rear + 1) % pool->queue_size == pool->front) {
        // Wait for space in the task queue
        pthread_cond_wait(&(pool->not_empty), &(pool->lock));
    }

    pool->task_queue[pool->rear].function = function;
    pool->task_queue[pool->rear].argument = argument;
    pool->rear = (pool->rear + 1) % pool->queue_size;

    pthread_cond_signal(&(pool->not_empty));
    pthread_mutex_unlock(&(pool->lock));
}

// Wait for all tasks to complete
void ThreadPoolWait(ThreadPool *pool) {
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

// Clean up the thread pool
void ThreadPoolCleanup(ThreadPool *pool) {
    if (!pool) {
        return;
    }

    pool->shutdown = true;

    pthread_cond_broadcast(&(pool->not_empty));

    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    free(pool->threads);
    free(pool->task_queue);
    free(pool);

    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->not_empty));
}
