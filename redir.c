#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void split_command(char *cmd, char **argv) {
    int i = 0;
    argv[i++] = strtok(cmd, " ");
    while ((argv[i++] = strtok(NULL, " ")) != NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-") != 0) {
        int fd_in = open(argv[1], O_RDONLY);
        if (fd_in < 0) {
            perror("Error opening input file");
            return 1;
        }
        if (dup2(fd_in, STDIN_FILENO) < 0) {
            perror("Error redirecting input");
            return 1;
        }
        close(fd_in);
    }

    if (strcmp(argv[3], "-") != 0) {
        int fd_out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0) {
            perror("Error opening output file");
            return 1;
        }
        if (dup2(fd_out, STDOUT_FILENO) < 0) {
            perror("Error redirecting output");
            return 1;
        }
        close(fd_out);
    }

    char *cmd_args[64];
    split_command(argv[2], cmd_args);
    if (fork() == 0) {
        execvp(cmd_args[0], cmd_args);
        perror("Exec failed");
        exit(1);
    }
    wait(NULL);
    return 0;
}
