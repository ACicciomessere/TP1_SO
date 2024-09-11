#include "dot_eich/sharedMemory.h"

shmADT createShm(char * name) {
      int fd = shm_open(name, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);

      if(fd == - 1) {
            perror("Error creating shared memory");
            exit(EXIT_FAILURE);
      }

      if(ftruncate(fd, sizeof(shmCDT)) == -1) {
            perror("Error resizing shared memory");
            exit(EXIT_FAILURE);
      }

      shmADT shm = mmap(NULL, sizeof(shmCDT), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

      if(shm == MAP_FAILED) {
            perror("Error mapping shared memory");
            exit(EXIT_FAILURE);
      }

      if(sem_init(&shm->semaphore, 1, 1) == -1) {
            perror("Error creating semaphore");
            exit(EXIT_FAILURE);
      }

      shm->name = name;
      shm->write_offset = 0;
      shm->read_offset = 0;

      return shm;
}

shmADT connectShm(char * shm_name) {
      int fd = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);

      if(fd == - 1) {
            perror("Error connecting to shared memory");
            exit(EXIT_FAILURE);
      }

      shmADT shm_map = mmap(NULL, sizeof(shmCDT), PROT_WRITE, MAP_SHARED, fd, 0);

      if(shm_map == MAP_FAILED) {
            perror("Error mapping shared memory");
            exit(EXIT_FAILURE);
      }

      if(shm_map == NULL) {
            finishShm(shm_name);
            exit(EXIT_FAILURE);
      }

      if(sem_init(&shm_map->semaphore, 1, 1) == -1) {
            perror("Error creating semaphore");
            exit(EXIT_FAILURE);
      }

      return shm_map;
}

void writeShm(shmADT shm, char * msg, int size, sem_t sem) {
      if(size > BUFFER_SIZE) {
            perror("Message too big for the buffer");
            exit(EXIT_FAILURE);
      }

      if(shm == NULL || msg == NULL) {
            perror("Invalid shared memory or buffer");
            exit(EXIT_FAILURE);
      }

      int i;

      for(i = 0; i < BUFFER_SIZE && shm->buffer[shm->write_offset] != '\0'; i++) {
            shm->buffer[shm->write_offset++] = msg[i];
      }

      shm->buffer[shm->write_offset++] = msg[i];

      sem_post(&shm->semaphore);

      return;
}

void readShm(shmADT shm, char * buffer) {
      if (sem_wait(&shm->semaphore) != 0) {
            perror("Failed to lock semaphore before accessing shared memory");
            exit(EXIT_FAILURE);
      }

      int i; 

      for(i = 0; i < BUFFER_SIZE && shm->buffer[shm->read_offset] != 0; i++) {
            buffer[i] = shm->buffer[shm->read_offset++];
      }

      shm->read_offset++;

      buffer[i] = 0;

      return;
}


void finishShm(char * shm_name) {
      if(shm_unlink(shm_name) == -1) {
            perror("Error deleting shared memory");
            exit(EXIT_FAILURE);
      }
      return;
}

