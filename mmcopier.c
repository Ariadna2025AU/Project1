#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_FILENAME 100

// Free paths allocated for source and destination
void free_paths(char **paths) {
    free(paths[0]);
    free(paths[1]);
    free(paths);
}

// Thread function: copy one file from source to destination
void *copy_file(void *arg) {
    char **paths = (char **)arg;
    char *source_path = paths[0];
    char *dest_path = paths[1];

    FILE *src = fopen(source_path, "r");
    if (!src) {
        perror("Error opening source file");
        free_paths(paths);
        pthread_exit(NULL);
    }

    FILE *dst = fopen(dest_path, "w");
    if (!dst) {
        perror("Error opening destination file");
        fclose(src); // ✅ fix: close src if dst fails
        free_paths(paths);
        pthread_exit(NULL);
    }

    int ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dst);
    }

    fclose(src);
    fclose(dst);
    free_paths(paths);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <n> <source_dir> <destination_dir>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n < 2 || n > 10) {
        fprintf(stderr, "Number of files must be between 2 and 10.\n");
        return 1;
    }

    char *src_dir = argv[2];
    char *dst_dir = argv[3];

    pthread_t threads[n];

    // Create threads: each copies one file
    for (int i = 0; i < n; i++) {
        char **paths = malloc(sizeof(char *) * 2);
        if (!paths) {
            perror("malloc failed");
            return 1;
        }

        paths[0] = malloc(MAX_FILENAME);
        paths[1] = malloc(MAX_FILENAME);
        if (!paths[0] || !paths[1]) {
            perror("malloc failed");
            free_paths(paths);
            return 1;
        }

        snprintf(paths[0], MAX_FILENAME, "%s/source%d.txt", src_dir, i + 1);
        snprintf(paths[1], MAX_FILENAME, "%s/source%d.txt", dst_dir, i + 1);

        if (pthread_create(&threads[i], NULL, copy_file, paths) != 0) {
            perror("Failed to create thread");
            free_paths(paths);
            return 1;
        }
    }

    // Wait for all threads
    for (int i = 0; i < n; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    return 0;
}
