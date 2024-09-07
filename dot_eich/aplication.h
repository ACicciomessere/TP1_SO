#ifndef APLICATION_H
#define APLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>

#define BUFFER_SIZE 1024
#define SLAVES_AMOUNT 5
#define INITIAL_FILES_AMOUNT 100
#define ERROR -1

typedef struct {
    pid_t pid;
    int pipe_to_slave[2];
    int pipe_to_master[2];
} pipe_master_slaves;

#endif