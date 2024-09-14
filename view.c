#include "dot_eich/application.h"
#include "dot_eich/sharedMemory.h"

int main(int argc, char *argv[]) {
      char shmName[NAME_SIZE];
      if (argc > 1) {
            strncpy(shmName, argv[1], NAME_SIZE - 1);
            shmName[NAME_SIZE - 1] = '\0';
      } else {
            scanf("%31s", shmName);
      }

      shmADT shm = connectShm(shmName);

      char buffer[BUFFER_SIZE];

      while (1) {
            readShm(shm, buffer);
            printf(shm->name);
            if (buffer[0] == '\0') break;
            printf("%s\n", buffer);
      }

      munmap(shm, sizeof(shmCDT));

      //finishShm(shm->name);

      return 0;
}