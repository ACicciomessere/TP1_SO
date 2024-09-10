#include <aplication.h>

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE] = {0};
    int files_count= argc - 1;
    int cant_slaves = getSlavesAmount(files_count); 
    pipe_master_slaves pipes[cant_slaves - 1];

    if (files_count < 1) {
        printf("No files passed as arguments\n");
        return -1;
    }

    for(int i=0; i<cant_slaves ; i++){
        if(pipe(pipes[i].pipe_to_slave) == -1 || pipe(pipes[i].pipe_to_master) == -1){
           perror("Couldnt create pipe.\n");
           exit(ERROR); 
        }

        if((pipes[i].pid = fork()) == 0 ){
           int j = i -1;
           while (j >= 0){                                       
                close(pipes[j].pipe_to_slave[1]);
                close(pipes[j].pipe_to_master[0]);
                j--;
           } 
           createSlave(pipes[j].pipe_to_slave[0], pipes[j].pipe_to_slave[1], pipes[j].pipe_to_master[0], pipes[j].pipe_to_master[1]);
        }
        close(pipes[i].pipe_to_slave[0]);
        close(pipes[i].pipe_to_master[1]);
    }
 
}

void createSlave(int fd_ms_r, int fd_ms_w, int fd_sm_r, int fd_sm_w) {

    close(fd_ms_w);
    close(fd_sm_r);
    close(STDOUT_FILENO);
    dup(fd_sm_w);
    close(STDIN_FILENO);
    dup(fd_ms_r);

    close(fd_sm_w);
    close(fd_ms_r);
    execve("./slave.eje", "./salve.eje", NULL);
}

int getSlavesAmount(int files_amount){
    if(files_amount < INITIAL_FILES_AMOUNT){
        return SLAVES_AMOUNT;
    }

    return (int) (files_amount/INITIAL_FILES_AMOUNT) * SLAVES_AMOUNT;
}

void sendFilesToSlaves(char * files[], int files_amount, int slaves_amount, pipe_master_slaves pipes[]){

    char w_buff[BUFFER_SIZE];
    
    for(int i = 0; i < slaves_amount && files_amount > 0; i++){

        sprintf(w_buff,"%s",files[files_amount--]);
        int r_write = write(pipes[i].pipe_to_master, w_buff, sizeof(w_buff));  //puede ser q sea el pipe_to_slave todo 

        if (r_write < 0){                                                  
            perror("write");
            exit(1);
        }
    } 
}