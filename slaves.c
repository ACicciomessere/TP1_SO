#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>

#define SLAVE_SIZE 512
#define HASH_SIZE 33


int main() {
    int i;
    char f_buffer[SLAVE_SIZE] = {0};
    char hash[SLAVE_SIZE] = {0}; 
    pid_t pid = getPid();
    int can_read = 1;
    char *string;

    while(can_read > 0){
        can_read = read(0, f_buffer, SLAVE_SIZE);

        if(can_read == 0){
            continue;
        }

        buffer[can_read - 1] = '\0';

        int process = hash(f_buffer, hash);

        if(process == -1){
            printf("Error hashing file\n");
            return -1;
        }

        char file_name = bname(f_buffer);

        printf("File: %s, Hash: %s\n, Slave ID: %s\n", file_name, hash, pid);

        strcpy(string, "File: %s, Hash: %s\n, Slave ID: %s\n", file_name, hash, pid);

        write(1, string, strlen(string));

    }
     
    return 0;

}

int hash(char *file, char *buffer) {
    char * fun_command = malloc(300*sizeof(char));

    strcpy(fun_command, "md5sum /");
    strcat(fun_command, file);

    FILE *pipe = popen(command, "r");
    if( pipe == NULL ){
        printf("Error opening pipe\n");
        return -1;
    }

    char h_buffer[HASH_SIZE];

    fgets(h_buffer, sizeof(h_buffer), pipe);
    pclose(pipe);
    strcpy(buffer, h_buffer);

    free(fun_command);
    return 0;
}
