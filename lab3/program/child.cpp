#include <iostream>

#include "func.h"
using namespace std;

int main() {
  int fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, S_IRUSR | S_IWUSR);

  char* shared_memory =
      (char*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  while (strlen(shared_memory) == 0) {
    // Можно добавить небольшую задержку, чтобы не нагружать процессор
  }
  char fname_nums[256];
  char fname[100];
  char nums[100];
  strncpy(fname_nums, shared_memory, sizeof(fname_nums));

  char* token = strtok(fname_nums, "\n");
  if (token != NULL) {
    strncpy(fname, token, sizeof(fname));
    fname[sizeof(fname) - 1] = '\0';
  } else {
    printf("Ошибка разбора строки\n");
    return 1;
  }
  token = strtok(NULL, "\n");
  if (token != NULL) {
    strncpy(nums, token, sizeof(nums));
    nums[sizeof(nums) - 1] = '\0';
  } else {
    printf("Ошибка разбора строки\n");
    return 1;
  }
  // Читаем имя файла из канала
  FILE* file = fopen(fname, "w");
  if (file == NULL) {
    fprintf(stderr, "Failed to open the file\n");
    return 1;
  }

  int suma = sum_numbers(nums);
  fprintf(file, "%d", suma);

  sprintf(shared_memory, "%d", suma);
  close(fd);
  munmap(shared_memory, BUFFER_SIZE);
}