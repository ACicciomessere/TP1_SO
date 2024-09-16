#include "dot_eich/slave.h"

int main() {

    char buff[SLAVE_SIZE] = {0};
    char hash[SLAVE_SIZE] = {0};
    char* str = malloc(sizeof(char)*300);

    //chequear null str
    char* bname;
    int read_bytes = 1;
    pid_t pid = getpid();
    while(read_bytes > 0){
        read_bytes = read(0, buff, sizeof(buff));
        if (read_bytes <= 0)
            continue;

        buff[read_bytes]='\0';

        int status = hash_func(buff, hash);
        if ( status != 0 ){
            perror("Hashing error");
            exit(EXIT_FAILURE);
        }
        bname = basename(buff);
        sprintf( str, "File: %s - Md5: %s - Slave Pid: %d\n", bname, hash, pid);

        write(1, str, strlen(str));
        fsync(1);
    }
    if ( read_bytes < 0 ){
        exit(1);
    }

    free(str);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    exit(EXIT_SUCCESS);

}

int hash_func( char* file, char* hash){

    char* command = malloc(300*sizeof(char));
    if ( command == NULL ){
        return EXIT_FAILURE;
    }

    strcpy(command,"md5sum ");
    strcat(command,file);

    fprintf(stderr, "|%s|\n", command);

    FILE* pipe = popen(command, "r");
    if ( pipe == NULL ){
        perror("Error");
        return EXIT_FAILURE;
    }

    char buff[33];
    fgets(buff, sizeof(buff), pipe);

    strcpy(hash, buff);

    pclose(pipe);
    free(command);

    return EXIT_SUCCESS;
}
