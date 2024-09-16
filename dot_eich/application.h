#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include "sharedMemory.h"
#include <signal.h>

#define BUFFER_SIZE 1024
#define SLAVES_AMOUNT 5
#define INITIAL_FILES_AMOUNT 100
#define ERROR -1

#define READ 0
#define WRITE 1

typedef struct {
    pid_t pid;
    int pipe_to_slave[2];
    int pipe_to_master[2];
} pipe_master_slaves;

int getSlavesAmount(int files_amount);
void sendFilesToSlaves(char * files[], int files_amount, int slaves_amount, pipe_master_slaves pipes[], shmADT shm);
void createSlave(int fd_to_slave_read, int fd_to_master_write);

#endif