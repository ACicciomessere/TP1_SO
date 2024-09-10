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

    shmADT shm = createShm("shm");

    for(int i=0; i<cant_slaves ; i++){
        if(pipe(pipes[i].pipe_to_slave) == -1 || pipe(pipes[i].pipe_to_master) == -1){
           perror("Couldnt create pipe.\n");
           exit(ERROR); 
        }

        if((pipes[i].pid = fork()) == 0 ){
           int j = i -1;
           while (j >= 0){                                       
                close(pipes[j].pipe_to_slave[WRITE]);
                close(pipes[j].pipe_to_master[READ]);
                j--;
           } 
           createSlave(pipes[j].pipe_to_slave[READ], pipes[j].pipe_to_slave[WRITE], pipes[j].pipe_to_master[READ], pipes[j].pipe_to_master[WRITE]);
        }
    }

    sendFilesToSlaves(argv + 1, files_count, cant_slaves, pipes, shm);

    for(int i = 0; i < cant_slaves; i++){
        close(pipes[i].pipe_to_slave[READ]);
        close(pipes[i].pipe_to_master[WRITE]);
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

char * memoryPointer(char * pointer, char * mem, sem_t * sem){
    while(*mem != '\0'){
        sem_wait(sem);
        *pointer = *mem;
        pointer++;
        mem++;
    }
    sem_post(sem);
    return ++pointer;
}

void sendFilesToSlaves(char * files[], int files_amount, int slaves_amount, pipe_master_slaves pipes[], shmADT shm){
    int files_per_slave = files_amount/slaves_amount;
    char w_buff[BUFFER_SIZE];
    int files_read = 0;

    fd_set read_fds;
    int max_fd = -1;
    
    for(int i = 0; i < slaves_amount && files_amount > 0; i++){

        sprintf(w_buff,"%s",files[files_amount--]);
        int r_write = write(pipes[i].pipe_to_master[WRITE], w_buff, sizeof(w_buff));  //puede ser q sea el pipe_to_slave todo 

        if (r_write < 0){                                                  
            perror("write");
            exit(1);
        }

    } 

    while (files_read < files_amount) {
        FD_ZERO(&read_fds);

        for (int j = 0; j < slaves_amount; j++) {
            FD_SET(pipes[j].slave_to_master[READ], &read_fds);
            max_fd = (pipes[j].slave_to_master[READ] > max_fd) ? pipes[j].slave_to_master[READ] : max_fd;
        }

        int select_ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (select_ready < 0) {
            perror("select ready");
            return -1;
        } else if (select_ready == 0) {
            continue;
        } else {
            for (int j = 0; j < slaves_amount && i < files_amount; j++) {
                if (FD_ISSET(pipes[j].slave_to_master[READ], &read_fds)) {
                    ssize_t len = read(pipes[j].slave_to_master[READ], w_buff, sizeof(w_buff));

                    if (len < 0) {
                        perror("read");
                        return -1;
                    }

                    w_buff[len] = '\0';
                    writeShm(shm, w_buff, strlen(w_buff), shm->semaphore); 
                    fprintf(result,"%s",w_buff);
                    write(pipes[j].master_to_slave[WRITE], files[i], strlen(files[i]));
                    files_read++;
                }
            }
        }
    }
}