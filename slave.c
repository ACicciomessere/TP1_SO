#include "dot_eich/slave.h"

int main() {

    char f_buffer[SLAVE_SIZE];
    char hash[SLAVE_SIZE]; 
    pid_t pid = getpid();
    int can_read = 1;
    // char *string = malloc(sizeof(char)*400);
    char string[400];

    while (1) {
        int total_read = 0;
        while (total_read < SLAVE_SIZE - 1) {
            int n = read(STDIN_FILENO, &f_buffer[total_read], 1);
            if (n <= 0) {
                can_read = n;
                break;
            }
            if (f_buffer[total_read] == '\0') {
                break;
            }
            total_read += n;
        }

        if (can_read <= 0) {
            break;
        }

        f_buffer[total_read] = '\0';

        char* file_name = basename(f_buffer);
        snprintf(string, strlen(string), "File: %s, Hash: %s, Slave ID: %d\n", file_name, hash, pid);
        write(STDOUT_FILENO, string, strlen(string) + 1);  // Include null terminator
    }
     
    if(can_read < 0){
        return -1;
    } 

    // free(string);
    //cerrar la entrada y salida de pipes ? todo
    return 0;
}

int hash_func(char *file, char *buffer) {

    char * fun_command = malloc(sizeof(char)*400);
    if (fun_command == NULL) {
            perror("malloc");
            return -1;
        }

    strcpy(fun_command, "md5sum \"");
    strcat(fun_command, file);
    strcat(fun_command,"\" 2>/dev/null");

    FILE* pipe = popen(fun_command, "r");           
    if(pipe == NULL){
        perror("Error opening pipe\n");
        free(fun_command);
        return -1;
    }

    char h_buffer[HASH_SIZE * 2];
    buffer[HASH_SIZE - 1] = '\0';                   

    if (fgets(h_buffer, sizeof(h_buffer), pipe) == NULL) {
        pclose(pipe);
        free(fun_command);
        return -1;
    }
    pclose(pipe);
    strncpy(buffer, h_buffer, 32);
    buffer[32] = '\0';
    free(fun_command);
    return 0;


}
