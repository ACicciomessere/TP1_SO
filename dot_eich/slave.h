#ifndef SLAVE_H
#define SLAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#define SLAVE_SIZE 512
#define HASH_SIZE 33  

int hash_func(char *file, char *buffer);


#endif