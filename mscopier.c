#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 20
#define MAX_LINE_LENGTH 1024

// Struct for queue item
typedef struct {
    int line_num;
    char *line;
} queue_item;

queue_item queue[MAX_QUEUE_SIZE];
int front = 0, rear = 0, count = 0;

FILE *src_file;
FILE *dst_file;
int n_threads;
int reader_done = 0;
int next_to_write = 0;

// Synchronization
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_write = PTHREAD_COND_INITIALIZER;

// Safe replacement for strdup (not available on all compilers)
char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup == NULL) {
        return NULL;
    }
    memcpy(dup, s, len);
    return dup;
}

// Enqueue
void enqueue(queue_item item) {
    queue[rear] = item;
    rear = (rear + 1) % MAX_QUEUE_SIZE;
    count++;
    pthread_cond_signal(&queue_not_empty);
}

// Dequeue
queue_item dequeue() {
    queue_item item = queue[front];
    front = (front + 1) % MAX_QUEUE_SIZE;
    count--;
    pthread_cond_signal(&queue_not_full);
    return item;
}

// Reader thread: reads lines and enqueues them
void *reader_thread(void *arg) {
    char buffer[MAX_LINE_LENGTH];
    int line_num = 0;

    while (fgets(buffer, MAX_LINE_LENGTH, src_file)) {
        char *line = my_strdup(buffer);
        if (!line) {
            perror("malloc failed in my_strdup");
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&queue_mutex);
        while (count >= MAX_QUEUE_SIZE) {
            pthread_cond_wait(&queue_not_full, &queue_mutex);
        }
        queue_item item = {line_num++, line};
        enqueue(item);
        pthread_mutex_unlock(&queue_mutex);
    }

    pthread_mutex_lock(&queue_mutex);
    reader_done = 1;
    pthread_cond_broadcast(&queue_not_empty);
    pthread_mutex_unlock(&queue_mutex);

    return NULL;
}

// Writer thread
void *writer_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);

        while (count == 0 && !reader_done) {
            pthread_cond_wait(&queue_not_empty, &queue_mutex);
        }

        if (count == 0 && reader_done) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }

        queue_item item = dequeue();

        // Wait until it's the right turn to write
        while (item.line_num != next_to_write) {
            pthread_cond_wait(&can_write, &queue_mutex);
        }

        // Write to file in correct order
        fprintf(dst_file, "%s", item.line);
        free(item.line);
        next_to_write++;
        pthread_cond_broadcast(&can_write);

        pthread_mutex_unlock(&queue_mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <n_threads> <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    n_threads = atoi(argv[1]);
    if (n_threads < 1 || n_threads > 10) {
        fprintf(stderr, "Number of writer threads must be between 1 and 10.\n");
        return 1;
    }

    src_file = fopen(argv[2], "r");
    if (!src_file) {
        perror("Source file error");
        return 1;
    }

    dst_file = fopen(argv[3], "w");
    if (!dst_file) {
        perror("Destination file error");
        fclose(src_file);
        return 1;
    }

    pthread_t reader;
    pthread_t writers[n_threads];

    // Start reader
    if (pthread_create(&reader, NULL, reader_thread, NULL) != 0) {
        perror("Failed to create reader thread");
        fclose(src_file);
        fclose(dst_file);
        return 1;
    }

    // Start writers
    for (int i = 0; i < n_threads; i++) {
        if (pthread_create(&writers[i], NULL, writer_thread, NULL) != 0) {
            perror("Failed to create writer thread");
            fclose(src_file);
            fclose(dst_file);
            return 1;
        }
    }

    // Join
    pthread_join(reader, NULL);
    for (int i = 0; i < n_threads; i++) {
        pthread_join(writers[i], NULL);
    }

    fclose(src_file);
    fclose(dst_file);

    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_not_full);
    pthread_cond_destroy(&queue_not_empty);
    pthread_cond_destroy(&can_write);

    return 0;
}
