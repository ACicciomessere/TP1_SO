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

      shmADT shm = connectShm(shmName);

      char buffer[BUFFER_SIZE];

      int i = shm->iter;
      while (i > 0) {
            if(getFlag(shm)){
                  if(sem_getvalue(shm, &shm->semaphore) == -1){
                        break;
                  }
            }
      
            printf("%d\n", i);
            readShm(shm, buffer);
            if (buffer[0] == '\0'){
                  printf("Aaaa");
                  break;
            }
            printf("%s\n", buffer);
            i--;
      }

      finishShm(shm);

      return 0;
}