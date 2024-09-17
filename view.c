#include "dot_eich/application.h"
#include "dot_eich/sharedMemory.h"

int main(int argc, char *argv[]) {
      char shmName[NAME_SIZE];
      if (argc > 1) {
            strncpy(shmName, argv[1], NAME_SIZE - 1);
            shmName[NAME_SIZE - 1] = '\0';
      } else {
            printf("me ?? nothing \n");
            scanf("%31s", shmName);
      }

      char ready[20];
      while (fgets(ready, sizeof(ready), stdin)) {
            if(strcmp(ready, "SHM_READY\n") == 0)
            break;
      }

      char buffer[BUFFER_SIZE];
      shmADT shm = connectShm(shmName);

      int i = shm->iter;
      while (i > 0) {
            if(getFlag(shm)){
                  if(sem_getvalue(shm, &shm->semaphore) == -1){
                        break;
                  }
            }
            readShm(shm, buffer);
            if (buffer[0] == '\0'){
                  break;
            }
            printf("%s\n", buffer);
            i--;
      }

      finishShm(shm);

      return 0;
}