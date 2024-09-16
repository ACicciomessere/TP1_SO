FLAGS := -Wall -std=c99 -D_XOPEN_SOURCE=500 -pedantic -g -lc  -fsanitize=address -lpthread
CC := gcc

all: sharedMemory application view slaves

sharedMemory: sharedMemory.c
	$(CC) $(FLAGS) sharedMemory.c -o sharedMemory 

application: application.c
	$(CC) $(FLAGS) application.c -o application sharedMemory.c

view: view.c
	$(CC) $(FLAGS) view.c -o view sharedMemory.c

slaves: slaves.c
	$(CC) $(FLAGS) slaves.c -o slaves 

clean:
	rm -f sharedMemory application view slaves
