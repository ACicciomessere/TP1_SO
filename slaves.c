#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>

#define SLAVE_SIZE 512
#define HASH_SIZE 33


int main() {
    char f_buffer[SLAVE_SIZE] = {0};
    char hash[SLAVE_SIZE] = {0}; 
    pid_t pid = getPid();
    int can_read = 1;
    char *string;

    while(can_read > 0){
        can_read = read(0, f_buffer, SLAVE_SIZE);

        if(can_read == 0){
            continue;                                   // huuh ??? todo 
        }

        f_buffer[can_read - 1] = '\0';

        int process = hash_func(f_buffer, hash);

        if(process == -1){
            printf("Error hashing file\n");
            return -1;
        }

        char file_name = bname(f_buffer);

        snprintf(string, sizeof(string), "File: %s, Hash: %s, Slave ID: %d\n", file_name, hash, pid);
        printf("%s \n", string);

        //strcpy(string, "File: %s, Hash: %s\n, Slave ID: %s\n", file_name, hash, pid);                creo q lo correcto es la func de arriba todo
        //write(1, string, strlen(string));                                                            esto tmb chau

    }
     
    return 0;

}

int hash_func(char *file, char *buffer) {
    char * fun_command = malloc(300*sizeof(char));
    if (fun_command == NULL) {
            perror("malloc");
            return -1;
        }

    strcpy(fun_command, "md5sum /");
    strcat(fun_command, file);

    FILE *pipe = popen(command, "r");                   //se referian a fun_command  ?? todo
    if( pipe == NULL ){
        printf("Error opening pipe\n");
        free(fun_command);
        return -1;
    }

    char h_buffer[HASH_SIZE];

    fgets(h_buffer, sizeof(h_buffer), pipe);
    pclose(pipe);
    strcpy(buffer, h_buffer);
    buffer[HASH_SIZE - 1] = '\0';                    // va??    todo

    free(fun_command);
    return 0;
}
