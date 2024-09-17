#include "dot_eich/slave.h"

int main() {

    int read_bytes = 1;
    char buffer[SLAVE_SIZE] = {0};
    char hash[HASH_SIZE] = {0};
    char* str = malloc(sizeof(char)*300);
    char* bname;
    pid_t pid = getpid();

    if(str == NULL){
        perror("Malloc error");
        exit(EXIT_FAILURE);
    }
   
    while(read_bytes > 0){
        read_bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (read_bytes <= 0)
            break;
        buffer[read_bytes]='\0';
        int status = hash_func(buffer, hash);
        if ( status != 0 ){
            perror("Hashing error");
            exit(EXIT_FAILURE);
        }

        bname = basename(buffer);
        sprintf( str, "File: %s - Md5: %s - Slave Pid: %d\n", bname, hash, pid);
        write(STDOUT_FILENO, str, strlen(str));
        fsync(1);
    }

    free(str);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    exit(EXIT_SUCCESS);

}

int hash_func( char* file, char* hash){

    char* command = malloc(300*sizeof(char));
    if ( command == NULL ){
        perror("malloc");
        return EXIT_FAILURE;
    }

    strcpy(command,"md5sum ");
    strcat(command,file);

    FILE* pipe = popen(command, "r");
    if ( pipe == NULL ){
        perror("Error opening pipe");
        return EXIT_FAILURE;
    }

    char buff[HASH_SIZE];
    fgets(buff, sizeof(buff), pipe);
    strcpy(hash, buff);
    pclose(pipe);
    free(command);

    return EXIT_SUCCESS;
}
