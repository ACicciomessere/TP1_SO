#include "dot_eich/application.h"
#include "dot_eich/sharedMemory.h"


int main (int argc, char *argv[]) {
    int files_count= argc -1;
    int cant_slaves = getSlavesAmount(files_count);
    pipe_master_slaves pipes[cant_slaves];


    if (files_count < 1) {
        perror("No files passed as arguments\n");
        return -1;
    }

    char * shmName = "shm";

    for(int i=0; i<cant_slaves ; i++){
        if(pipe(pipes[i].pipe_to_slave) == -1 || pipe(pipes[i].pipe_to_master) == -1){
           perror("Couldnt create pipe.\n");
           exit(ERROR); 
        }
        pipes[i].pid = fork();
        if(pipes[i].pid == 0 ) {
            for (int j = 0; j < i; j++) {
                    close(pipes[j].pipe_to_slave[READ]);
                    close(pipes[j].pipe_to_slave[WRITE]);
                    close(pipes[j].pipe_to_master[READ]);
                    close(pipes[j].pipe_to_master[WRITE]);
            }
            sleep(2);
            createSlave(pipes[i].pipe_to_slave[READ], pipes[i].pipe_to_master[WRITE]);            
        }
    }
    shmADT shm = createShm(shmName);
    shm->iter=files_count;

    sendFilesToSlaves(argv + 1, files_count, cant_slaves, pipes, shm);

    for(int i = 0; i < cant_slaves; i++){
        close(pipes[i].pipe_to_slave[READ]);
        close(pipes[i].pipe_to_master[WRITE]);
    }

    return 0;
}

void createSlave(int fd_to_slave_read, int fd_to_master_write) {

    dup2(fd_to_slave_read, STDIN_FILENO);
    close(fd_to_slave_read);

    dup2(fd_to_master_write, STDOUT_FILENO);
    close(fd_to_master_write);

    execl("./slave", "./slave", (char*) NULL);

    exit(EXIT_FAILURE);
}

int getSlavesAmount(int files_amount){
    if(files_amount < INITIAL_FILES_AMOUNT){
        return SLAVES_AMOUNT;
    }

    return (int) (files_amount/INITIAL_FILES_AMOUNT) * SLAVES_AMOUNT;
}

void sendFilesToSlaves(char *files[], int files_amount, int slaves_amount, pipe_master_slaves pipes[], shmADT shm) {
    char w_buff[BUFFER_SIZE];
    int files_sent = 0;
    int files_read = 0;
    fd_set read_fds;

    for(int i = 0; i < slaves_amount && files_sent < files_amount; i++){
        sprintf(w_buff, "%s", files[files_sent++]);
        int r_write = write(pipes[i].pipe_to_slave[WRITE], w_buff, strlen(w_buff));
        if (r_write < 0){                                                  
            perror("write");
            exit(1);
        }
    } 
   
    while (files_read < files_amount) {
        FD_ZERO(&read_fds);
        int max_fd = -1;

        for (int j = 0; j < slaves_amount; j++) {
            FD_SET(pipes[j].pipe_to_master[READ], &read_fds);
            if (pipes[j].pipe_to_master[READ] > max_fd)
                max_fd = pipes[j].pipe_to_master[READ];
        }
        
        int select_ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (select_ready < 0) {
            perror("select");
            return;
        } else if(select_ready == 0) {
            continue;
        }

        for (int j = 0; j < slaves_amount; j++) {
            if (FD_ISSET(pipes[j].pipe_to_master[READ], &read_fds)) {
                ssize_t len = read(pipes[j].pipe_to_master[READ], w_buff, sizeof(w_buff));
                if (len < 0) {
                    perror("read");
                    return;
                }
                w_buff[len] = '\0';
                writeShm(shm, w_buff, len);
                files_read++;

                if (files_sent < files_amount) {
                    sprintf(w_buff, "%s", files[files_sent++]);
                    int r_write = write(pipes[j].pipe_to_slave[WRITE], w_buff, strlen(w_buff));
                    if (r_write < 0){                                                  
                        perror("write");
                        exit(1);
                    }
                }
            }
        }
        setFlag(shm, 1);
    }
}
