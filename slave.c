#include "dot_eich/slave.h"

int main() {

    char f_buffer[SLAVE_SIZE];
    char hash[SLAVE_SIZE]; 
    pid_t pid = getpid();
    int can_read = 1;
    char *string = malloc(sizeof(char)*400);

    while(can_read > 0){
        can_read = read(0, f_buffer, SLAVE_SIZE);

        if(can_read == 0){
            continue;
        }

        f_buffer[can_read - 1] = '\0';

        int process = hash_func(f_buffer, hash);

        if(process == -1){
            perror("Error hashing file\n");
            return -1;
        }

        char* file_name = basename(f_buffer);

        int len = strlen(string);

        snprintf(string, len, "File: %s, Hash: %s, Slave ID: %d\n", file_name, hash, pid);
        printf("%s \n", string);
    }
     
    if(can_read < 0){
        return -1;
    } 

    free(string);
    //cerrar la entrada y salida de pipes ? todo
    return 0;
}

int hash_func(char *file, char *buffer) {

    char * fun_command = malloc(sizeof(char)*400);
    if (fun_command == NULL) {
            perror("malloc");
            return -1;
        }

    strcpy(fun_command, "md5sum /");                  //es esa barrita?? todo
    strcat(fun_command, file);

    FILE* pipe = popen(fun_command, "r");           
    if(pipe == NULL){
        perror("Error opening pipe\n");
        free(fun_command);
        return -1;
    }

    char h_buffer[HASH_SIZE];
    buffer[HASH_SIZE - 1] = '\0';                   

    fgets(h_buffer, sizeof(h_buffer), pipe);
    pclose(pipe);
    strcpy(buffer, h_buffer);
    free(fun_command);
    return 0;
}
