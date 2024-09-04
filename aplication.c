#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024
#define SLAVES_AMOUNT 5
#define INITIAL_FILES_AMOUNT 100

 //vamos a usar exit() o return ;    no creo q sea bueno ir combinando ambos todo

typedef struct {
    pid_t pid;
    int fd_read;
    int fd_write;
} slave;

slave slaves[SLAVES_AMOUNT]; 

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE] = {0};
    int files_count;

    if (argc < 2) {
        printf("Not enough arguments\n");
        return -1;
    }
    
    files_count = argc - 1;
}

int createSlave() {
    int pipe_array[2];
    pid_t pid;

    if(pipe(pipe_array)==-1){  //en el caso de error cerramos
        return -1;
    }

    pid = fork();

    if (pid == 0) { //estamos en el hijo
        // Child process
        execv("./slaves.c", argv);
    } 
    else if (pid == -1){ //tenemos un error al crear el hijo 
        printf("Not able to create child process\n");
        return -1;
    }

    return 0;   //porq no devuelve void ?? todo
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
        int r_write = write(slaves[i].fd_write, w_buff, sizeof(w_buff));

        if (r_write < 0){                                                  
            perror("write");
            exit(1);
        }
    }
   
}