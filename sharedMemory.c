#include <sharedMemory.h>

typedef struct shmCDT {
      char * name;
      char buffer[BUFFER_SIZE];
      int write_offset;
      int read_offset;
      sem_t semaphore;
} shmCDT;

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

shmAdt connectShm(char * shm) {
      //implementar
}

void writeShm(shmADT shm, char * msg, int size) {
      if(size > BUFFER_SIZE) {
            perror("Message too big for the buffer");
            exit(EXIT_FAILURE);
      }

      //Falta terminar la implementacion
}

void readShm(shmADT shm, char * buffer) {
      if (sem_wait(&shm->semaphore) != 0) {
            perror("Failed to lock semaphore before accessing shared memory");
            return EXIT_FAILURE;
      }

      //Falta terminar la implementacion
}


void finishShm(shmADT shm) {
      if(shm_unlink(shm->name) == -1) {
            perror("Error deleting shared memory");
            exit(EXIT_FAILURE);
      }

      if(sem_destroy(&shm->semaphore) == -1) {
            perror("Error destroying semaphore");
            exit(EXIT_FAILURE);
      }

      if(munmap(shm, sizeof(shmCDT)) == -1) {
            perror("Error unmapping shared memory");
            exit(EXIT_FAILURE);
      }
}

