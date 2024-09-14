#include "dot_eich/application.h"
#include "dot_eich/sharedMemory.h"

int main (int argc, char *argv[]) {
    int files_count= argc - 1;
    int cant_slaves = getSlavesAmount(files_count);
    pipe_master_slaves pipes[cant_slaves];


    if (files_count < 1) {
        printf("No files passed as arguments\n");
        return -1;
    }

    char * shmName = "shm";

    shmADT shm = createShm(shmName);        //Problema GCC

    for(int i=0; i<cant_slaves ; i++){
        if(pipe(pipes[i].pipe_to_slave) == -1 || pipe(pipes[i].pipe_to_master) == -1){
           perror("Couldnt create pipe.\n");
           exit(ERROR); 
        }

        if((pipes[i].pid = fork()) == 0 ){
        //    int j = i -1;
        //    while (j >= 0){                                       
        //         close(pipes[j].pipe_to_slave[WRITE]);
        //         close(pipes[j].pipe_to_master[READ]);
        //         j--;
        //    } 
        //    createSlave(pipes[j].pipe_to_slave[READ], pipes[j].pipe_to_slave[WRITE], pipes[j].pipe_to_master[READ], pipes[j].pipe_to_master[WRITE]);
        if((pipes[i].pid = fork()) == 0 ){
    // Child process
    // Close unused ends of all pipes
        for (int j = 0; j < cant_slaves; j++) {
            if (j != i) {
                close(pipes[j].pipe_to_slave[READ]);
                close(pipes[j].pipe_to_slave[WRITE]);
                close(pipes[j].pipe_to_master[READ]);
                close(pipes[j].pipe_to_master[WRITE]);
            }
        }
        // Close unused ends of the current pipe
        close(pipes[i].pipe_to_slave[WRITE]); // Child doesn't write to this pipe
        close(pipes[i].pipe_to_master[READ]); // Child doesn't read from this pipe

        // Now, the child will read from pipe_to_slave[READ], and write to pipe_to_master[WRITE]
        createSlave(pipes[i].pipe_to_slave[READ], pipes[i].pipe_to_master[WRITE]);

        // If exec fails
        perror("execve");
        exit(EXIT_FAILURE);
}
        }
    }

    sendFilesToSlaves(argv + 1, files_count, cant_slaves, pipes, shm);

    for(int i = 0; i < cant_slaves; i++){
        close(pipes[i].pipe_to_slave[READ]);
        close(pipes[i].pipe_to_master[WRITE]);
    }

    return 0;
}

// void createSlave(int fd_ms_r, int fd_ms_w, int fd_sm_r, int fd_sm_w) {
//     close(fd_ms_w);
//     close(fd_sm_r);
//     close(STDOUT_FILENO);
//     dup(fd_sm_w);
//     close(STDIN_FILENO);
//     dup(fd_ms_r);

//     close(fd_sm_w);
//     close(fd_ms_r);
//     char *const secArg[] = {"./slaves", NULL};
//     execve("./slave", secArg, NULL);
// }

void createSlave(int fd_to_slave_read, int fd_to_master_write) {
    // Redirect stdin to fd_to_slave_read
    dup2(fd_to_slave_read, STDIN_FILENO);
    close(fd_to_slave_read);

    // Redirect stdout to fd_to_master_write
    dup2(fd_to_master_write, STDOUT_FILENO);
    close(fd_to_master_write);

    // Execute the slave process
    char *const args[] = {"./slave", NULL};
    execve("./slave", args, NULL);

    // If execve fails
    perror("execve");
    exit(EXIT_FAILURE);
}

int getSlavesAmount(int files_amount){
    if(files_amount < INITIAL_FILES_AMOUNT){
        return SLAVES_AMOUNT;
    }

    return (int) (files_amount/INITIAL_FILES_AMOUNT) * SLAVES_AMOUNT;
}

void sendFilesToSlaves(char * files[], int files_amount, int slaves_amount, pipe_master_slaves pipes[], shmADT shm){
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
            FD_SET(pipes[j].pipe_to_master[READ], &read_fds);
            max_fd = (pipes[j].pipe_to_master[READ] > max_fd) ? pipes[j].pipe_to_master[READ] : max_fd;
        }

        int select_ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (select_ready < 0) {
            perror("select ready");
            return;
        } else if (select_ready == 0) {
            continue;
        } else {
            for (int j = 0, i = 0; j < slaves_amount && i < files_amount; j++) {
                if (FD_ISSET(pipes[j].pipe_to_master[READ], &read_fds)) {
                    ssize_t len = read(pipes[j].pipe_to_master[READ], w_buff, sizeof(w_buff));

                    if (len < 0) {
                        perror("read");
                        return;
                    }

                    w_buff[len] = '\0';
                    writeShm(shm, w_buff, strlen(w_buff), shm->semaphore);      //Problema GCC
                    write(pipes[j].pipe_to_slave[WRITE], files[i], strlen(files[i]));
                    files_read++;
                }
            }
        }
    }
}