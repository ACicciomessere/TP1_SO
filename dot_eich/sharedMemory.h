#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h> 

#define BUFFER_SIZE 1024;

typedef struct shmCDT * shmADT;

shmADT createShm(char * name);

shmAdt connectShm(char * shm) 

void writeShm(shmADT shm, char * msg, int size);

void readShm(shmADT shm, char * buffer);

void finishShm(shmADT shm);

#endif