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

      strncpy(shm->name, name, NAME_SIZE - 1);
      shm->name[NAME_SIZE - 1] = '\0'; 
      shm->write_offset = 0;
      shm->read_offset = 0;

      const char *ready_signal = "SHM_READY";
      write(STDOUT_FILENO, ready_signal, strlen(ready_signal));
      fflush(stdout);
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
            finishShm(shm_map);
            exit(EXIT_FAILURE);
      }

      strncpy(shm_map->name, shm_name, NAME_SIZE - 1);
      shm_map->name[NAME_SIZE - 1] = '\0';

      return shm_map;
}

void writeShm(shmADT shm, char * msg, int size) {
      if(size > BUFFER_SIZE) {
            perror("Message too big for the buffer");
            exit(EXIT_FAILURE);
      }

      if(shm == NULL || msg == NULL) {
            perror("Invalid shared memory or buffer");
            exit(EXIT_FAILURE);
      }

      if (sem_wait(&shm->semaphore) != 0) {
        perror("Failed to lock semaphore");
        exit(EXIT_FAILURE);
      }

      snprintf(&(shm->buffer[shm->write_offset]), size + 1, "%s", msg);
      shm->write_offset += size + 1;
      sem_post(&shm->semaphore);

      return;
}

void readShm(shmADT shm, char * buffer) {
      if (sem_wait(&shm->semaphore) != 0) {
            perror("Failed to lock semaphore before accessing shared memory");
            exit(EXIT_FAILURE);
      }
      
      shm->read_offset += sprintf(buffer, "%s", &(shm->buffer[shm->read_offset])) + 1;
      if (shm->read_offset >= shm->write_offset) {
        shm->read_offset = 0;
        shm->write_offset = 0;
      }

      return;
}

int getFlag(shmADT shm){
      return shm->flag;
}

void setFlag(shmADT shm, int val){
      shm->flag = val;
}


void finishShm(shmADT shm_name) {
      char shmName[NAME_SIZE];
      strcpy(shmName, shm_name->name);

      if(sem_destroy(&shm_name->semaphore) == -1) {
            perror("Error destroying semaphore");
            exit(EXIT_FAILURE);
      }

      if(munmap(shm_name, sizeof(*shm_name)) == -1) {
            perror("Error unmapping shared memory");
            exit(EXIT_FAILURE);
      }

      if(shm_unlink(shmName) == -1) {
            perror("Error unlinking shared memory");
            exit(EXIT_FAILURE);
      }
}

