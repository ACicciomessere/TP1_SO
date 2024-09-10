#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h> 


#define NAME_SIZE 32

typedef struct shmADT {
      char name[NAME_SIZE];
      char buffer[BUFFER_SIZE];
      int write_offset;
      int read_offset;
      sem_t semaphore;
} shmADT;

#define BUFFER_SIZE 1024;

typedef struct shmCDT * shmADT;

shmADT createShm(char * name);

shmAdt connectShm(char * shm) 

void writeShm(shmADT shm, char * msg, int size);

void readShm(shmADT shm, char * buffer);

void finishShm(shmADT shm);

#endif