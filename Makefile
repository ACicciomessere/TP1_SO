FLAGS := -Wall -std=c99 -D_XOPEN_SOURCE=500 -pedantic -g  -fsanitize=address -lpthread
CC := gcc

all: sharedMemory application view slave

sharedMemory:
	$(CC) -Wall -std=c99 -D_XOPEN_SOURCE=500  -pedantic -g -c -fsanitize=address -lpthread sharedMemory.c -o sharedMemory

application:
	$(CC) $(FLAGS) application.c -o application sharedMemory.c

view:
	$(CC) $(FLAGS) view.c -o view sharedMemory.c

slave:
	$(CC) $(FLAGS) slave.c -o slave

clean:
	rm -f sharedMemory application view slave
