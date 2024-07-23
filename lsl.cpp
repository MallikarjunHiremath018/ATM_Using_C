#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void list_files(const char *dir_path) {
    struct dirent *entry;
    DIR *dir = opendir(dir_path);
    blkcnt_t total_blocks = 0;

    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        total_blocks += file_stat.st_blocks;
    }

    printf("Total: %ld\n", (long)total_blocks); // Move total block count here

    // Reset directory stream and iterate over entries again
    rewinddir(dir);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat file_stat;
        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        struct passwd *pw = getpwuid(file_stat.st_uid);
        struct group *gr = getgrgid(file_stat.st_gid);

        printf("%c%c%c%c%c%c%c%c%c%c %ld %s %s %5ld ",
            S_ISDIR(file_stat.st_mode) ? 'd' : '-',
            file_stat.st_mode & S_IRUSR ? 'r' : '-',
            file_stat.st_mode & S_IWUSR ? 'w' : '-',
            file_stat.st_mode & S_IXUSR ? 'x' : '-',
            file_stat.st_mode & S_IRGRP ? 'r' : '-',
            file_stat.st_mode & S_IWGRP ? 'w' : '-',
            file_stat.st_mode & S_IXGRP ? 'x' : '-',
            file_stat.st_mode & S_IROTH ? 'r' : '-',
            file_stat.st_mode & S_IWOTH ? 'w' : '-',
            file_stat.st_mode & S_IXOTH ? 'x' : '-',
            (long)file_stat.st_nlink, pw->pw_name, gr->gr_name, (long)file_stat.st_size);

        struct tm *timeinfo = localtime(&file_stat.st_mtime);
        char time_buffer[80];
        strftime(time_buffer, 80, "%b %d %H:%M", timeinfo);
        printf("%s %s\n", time_buffer, entry->d_name);
    }

    closedir(dir);
}

int main() {
    list_files(".");
    return 0;
}

