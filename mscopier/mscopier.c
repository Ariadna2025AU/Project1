#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE 20
#define MAX_LINE_LENGTH 1024

char *queue[MAX_QUEUE_SIZE];
int front = 0;
int rear = 0;
int count = 0;

FILE *src_file;
FILE *dst_file;

int n_threads;
int readers_done = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

// Función para insertar en la cola
void enqueue(char *line) {
    queue[rear] = line;
    rear = (rear + 1) % MAX_QUEUE_SIZE;
    count++;
}

// Función para sacar de la cola
char *dequeue() {
    char *line = queue[front];
    front = (front + 1) % MAX_QUEUE_SIZE;
    count--;
    return line;
}

// Hilo lector
void *reader_thread(void *arg) {
    char buffer[MAX_LINE_LENGTH];

    while (1) {
        if (fgets(buffer, MAX_LINE_LENGTH, src_file) == NULL)
            break;

        char *line = strdup(buffer);

        pthread_mutex_lock(&queue_mutex);
        if (count < MAX_QUEUE_SIZE) {
            enqueue(line);
        } else {
            // In subtask 1, if queue is full, drop the line
            free(line);
        }
        pthread_mutex_unlock(&queue_mutex);
    }

    pthread_mutex_lock(&queue_mutex);
    readers_done++;
    pthread_mutex_unlock(&queue_mutex);
    return NULL;
}

// Hilo escritor
void *writer_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        if (count > 0) {
            char *line = dequeue();
            pthread_mutex_unlock(&queue_mutex);

            fprintf(dst_file, "%s", line);
            free(line);
        } else {
            // exit condition: all readers finished and queue empty
            if (readers_done == n_threads) {
                pthread_mutex_unlock(&queue_mutex);
                break;
            }
            pthread_mutex_unlock(&queue_mutex);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <n> <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    n_threads = atoi(argv[1]);
    if (n_threads < 2 || n_threads > 10) {
        fprintf(stderr, "Number of threads must be between 2 and 10.\n");
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

    pthread_t readers[n_threads];
    pthread_t writers[n_threads];

    for (int i = 0; i < n_threads; i++) {
        pthread_create(&readers[i], NULL, reader_thread, NULL);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_create(&writers[i], NULL, writer_thread, NULL);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < n_threads; i++) {
        pthread_join(writers[i], NULL);
    }

    fclose(src_file);
    fclose(dst_file);

    return 0;
}