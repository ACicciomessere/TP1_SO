#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1000000
#define NAME_SIZE 32
#define SMALL_BUFFER 20

typedef struct shmCDT {
      char name[NAME_SIZE];
      char buffer[BUFFER_SIZE];
      int write_offset;
      int read_offset;
      int flag;
      int iter;
      sem_t semaphore;
} shmCDT;


typedef struct shmCDT * shmADT;

shmADT createShm(char * name);

shmADT connectShm(char * shm);

void writeShm(shmADT shm, char * msg, int size);

void readShm(shmADT shm, char * buffer);

int getFlag(shmADT shm);

void setFlag(shmADT shm, int n);

void finishShm(shmADT shm_name);

#endif