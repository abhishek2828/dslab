#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#define MAX_THREADS 4
#define MAX_PATH 256

// Structure to hold file information
typedef struct {
    char filename[MAX_PATH];
    off_t size;
    uid_t uid;
} FileInfo;

// Structure for passing data to threads
typedef struct {
    char directory[MAX_PATH];
    FileInfo max_file;
    pthread_mutex_t *mutex;
} ThreadData;

// Function to find the maximum file in a given directory
void *find_max_file(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    struct dirent *entry;
    struct stat file_stat;
    char filepath[MAX_PATH];

    DIR *dir = opendir(data->directory);
    if (!dir) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Ignore '.' and '..' directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Build the full file path
        snprintf(filepath, sizeof(filepath), "%s/%s", data->directory, entry->d_name);

        // Get file statistics
        if (stat(filepath, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
            // Update max file if current file is larger
            pthread_mutex_lock(data->mutex);
            if (file_stat.st_size > data->max_file.size) {
                strcpy(data->max_file.filename, filepath);
                data->max_file.size = file_stat.st_size;
                data->max_file.uid = file_stat.st_uid;
            }
            pthread_mutex_unlock(data->mutex);
        }
    }

    closedir(dir);
    return NULL;
}

int main() {
    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];
    pthread_mutex_t max_file_mutex;
    char cwd[MAX_PATH];

    // Get the current working directory
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return EXIT_FAILURE;
    }

    // Initialize thread data and mutex
    pthread_mutex_init(&max_file_mutex, NULL);
    for (int i = 0; i < MAX_THREADS; ++i) {
        strncpy(thread_data[i].directory, cwd, MAX_PATH);
        thread_data[i].max_file.size = 0; // Initialize max file size to 0
        thread_data[i].mutex = &max_file_mutex;
    }

    // Create threads to process the directory
    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_create(&threads[i], NULL, find_max_file, &thread_data[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Find the global maximum file from all threads
    FileInfo global_max_file = { "", 0, 0 };
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (thread_data[i].max_file.size > global_max_file.size) {
            global_max_file = thread_data[i].max_file;
        }
    }

    // Print the result
    if (global_max_file.size > 0) {
        printf("The largest file: %s\n", global_max_file.filename);
        printf("Size: %lld bytes\n", (long long)global_max_file.size);
        printf("Owned by UID: %d\n", global_max_file.uid);
    } else {
        printf("No files found in the current directory.\n");
    }

    // Clean up
    pthread_mutex_destroy(&max_file_mutex);

    return 0;
}
