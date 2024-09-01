#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024
#define SLAVES_AMOUNT 5
#define INITIAL_FILES_AMOUNT 100

typedef struct {
    pid_t pid;
    int fd_read;
    int fd_write;
} slave;

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

}

int createSlave() {
    if(pipe(pipe_array[2])){
        exit(1);
    }

    pid = fork();

    if (pid == 0) {
        // Child process
        execv("./slaves.c", argv);
    } else {
        printf("Not able to create child process\n");
        return -1;
    }
}

int getSlavesAmount(int files_amount){
    if(files_amount < INITIAL_FILES_AMOUNT){
        return SLAVES_AMOUNT;
    }

    return (int) (files_amount/INITIAL_FILES_AMOUNT) * SLAVES_AMOUNT;
}

void sendFilesToSlaves(char * files[], int files_amount, int slaves_amount ){

    char w_buff[BUFFER_SIZE];
    
    for(int i = 0; i < slaves_amount && files_amount > 0; i++){

        sprintf(w_buff,"%s",files[files_amount--]);
        int r_write = write(slave[i].fd_write, w_buff, sizeof(w_buff));

        if (wr < 0){
            perror("write");
            exit(1);
        }
    }

    

    
}