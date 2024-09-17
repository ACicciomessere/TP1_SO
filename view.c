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

      int i = 0;

      while (1) {
            readShm(shm, buffer);
            if (buffer[0] == '\0') break;
            printf("%s\n", buffer);
      }

      finishShm(shm);

      return 0;
}