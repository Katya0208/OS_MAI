#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHARED_MEMORY_NAME "/my_shared_memory"
#define BUFFER_SIZE 256
#include <iostream>

int sum_numbers(char input_nums[100]);