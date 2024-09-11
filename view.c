#include "dot_eich/application.h"
#include "dot_eich/sharedMemory.h"

int main(int argc, char *argv[]) {
      char shmName[NAME_SIZE];
      if (argc > 1) {
            strncpy(shmName, argv[1], NAME_SIZE - 1);
      } else {
            scanf("%8s", shmName);
      }

      shmADT shm = connectShm(shmName);

      char buffer[BUFFER_SIZE] = {0};

      while (1) {
            readShm(shm, buffer);
            if (buffer[0] == '\0') break;
            printf("%s\n", buffer);
      }

      finishShm(shm->name);

      return 0;
}