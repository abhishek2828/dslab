#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>

typedef struct {
    char username[256];
    off_t size;
} FileInfo;

// Function to get file size and owner
FileInfo getFileOwnerAndSize(const char *fileName) {
    struct stat fileStat;
    FileInfo fileInfo;

    if (stat(fileName, &fileStat) == 0) {
        struct passwd *pw = getpwuid(fileStat.st_uid);
        if (pw) {
            strcpy(fileInfo.username, pw->pw_name);
            fileInfo.size = fileStat.st_size;
        }
    }
    return fileInfo;
}

// Mapper function: Reads directory and emits file info
int mapper(FileInfo *fileInfos) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    if ((dir = opendir(".")) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) { // Regular file
                fileInfos[count++] = getFileOwnerAndSize(entry->d_name);
            }
        }
        closedir(dir);
    } else {
        perror("Could not open current directory");
        return -1;
    }
    return count;
}

// Reducer function: Finds maximum size and users who own those files
void reducer(FileInfo *fileInfos, int count) {
    off_t maxSize = 0;

    // First pass: Find maximum file size
    for (int i = 0; i < count; i++) {
        if (fileInfos[i].size > maxSize) {
            maxSize = fileInfos[i].size;
        }
    }

    // Second pass: Print users owning files of maximum size
    printf("Users owning files of maximum size (%lld bytes):\n", (long long) maxSize);
    for (int i = 0; i < count; i++) {
        if (fileInfos[i].size == maxSize) {
            printf("%s\n", fileInfos[i].username);
        }
    }
}

int main() {
    FileInfo fileInfos[1024];
    int count = mapper(fileInfos);

    if (count > 0) {
        reducer(fileInfos, count);
    }

    return 0;
}

