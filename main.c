#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <errno.h>

void find_files(char *root_path, int from_size, int to_size);

char *get_full_path(char *parent, char *name);

void print_error(const char *s_name, const char *msg, const char *desc);

int isNumber(char *str);

int counter = 0;
char *program_name;
FILE *to_write;

int main(int argc, char *argv[]) {
    program_name = basename(argv[0]);
    if (argc < 5) {
        print_error(program_name, "Incorrect amount of arguments.", 0);
        return 1;
    }
    char *dir_name = realpath(argv[1], NULL);
    if (dir_name == NULL) {
        print_error(program_name, "Error opening directory:", argv[1]);
        return 1;
    }
    if (!isNumber(argv[2]) || !isNumber(argv[3])) {
        print_error(program_name, "Incorrect sizes.", NULL);
        return 1;
    }
    if ((to_write = fopen(argv[4], "w")) == NULL) {
        print_error(program_name, "Can't open file:", argv[4]);
        return 1;
    }
    fprintf(to_write, "%-100s     %-30s     %-10s\n", "PATH:", "NAME:", "SIZE:");
    find_files(dir_name, atoi(argv[2]), atoi(argv[3]));
    if (fclose(to_write) == -1)
        print_error(program_name, "Error closing file:", realpath(argv[2], NULL));
    printf("Amount of checked files: %d\n", counter);
    return 0;

}

int isNumber(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9')
            return 0;
    }
    return 1;
}

void find_files(char *root_path, int from_size, int to_size) {
    struct dirent *structdirent;
    char *path;
    errno = 0;
    DIR *curr_dir;
    if ((curr_dir = opendir(root_path)) == NULL) {
        print_error(program_name, strerror(errno), NULL);
    }
    while ((structdirent = readdir(curr_dir))) {
        path = get_full_path(root_path, structdirent->d_name);
        if (structdirent->d_type == DT_DIR &&
            strcmp(".", structdirent->d_name) && strcmp("..", structdirent->d_name)) {
            find_files(path, from_size, to_size);
        } else if (structdirent->d_type == DT_REG) {
            struct stat *buf = (struct stat *) calloc(1, sizeof(struct stat));
            if (stat(path, buf) == -1) {
                print_error(program_name, strerror(errno), NULL);
            }
            if (buf->st_size > from_size && buf->st_size <= to_size) {
                fprintf(to_write, "%-100s     %-30s     %-10ld\n", path, structdirent->d_name, buf->st_size);
            }
            counter++;
        }
    }
    if (closedir(curr_dir) == -1) {
        print_error(program_name, strerror(errno), NULL);
    }
}

char *get_full_path(char *parent, char *name) {
    char *full_path = (char *) calloc
            (strlen(parent) + strlen(name) + 2, sizeof(char));
    strcpy(full_path, parent);
    strcat(full_path, "/");
    strcat(full_path, name);
    return full_path;
}

void print_error(const char *s_name, const char *msg, const char *desc) {
    fprintf(stderr, "%s: %s %s\n", s_name, msg, (desc) ? desc : "");
}