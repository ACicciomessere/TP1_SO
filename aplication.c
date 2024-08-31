#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024
#define SLAVES_AMOUNT 5

int main(int argc, char *argv[]) {
    pid_t pid;
    char buffer[BUFFER_SIZE] = {0};
    int files_count;
    int pipe_array[2];

    if (argc < 2) {
        printf("Not enough arguments\n");
        return -1;
    }
    
    files_count = argc - 1;
    
    pid = fork();

    int create_slave() {
        if(pipe(pipe_array[2])){
            exit(1);
        }

        if (pid == 0) {
            // Child process
            execv("./slaves.c", argv);
        } else {
            printf("Not able to create child process\n");
            return -1;
        }
    }

}