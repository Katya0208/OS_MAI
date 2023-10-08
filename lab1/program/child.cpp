#include <iostream>

#include "func.h"
using namespace std;

int main() {
  char fname_nums[200];
  char fname[100];
  char nums[100];
  read(STDIN_FILENO, fname_nums, sizeof(fname_nums));
  char* token = strtok(fname_nums, "\n");
  if (token != NULL) {
    strncpy(fname, token, sizeof(fname));
    fname[sizeof(fname) - 1] =
        '\0';  // Убедимся, что строка города завершена нулевым символом
  } else {
    printf("Ошибка разбора строки\n");
    return 1;
  }
  token = strtok(NULL, "\n");
  if (token != NULL) {
    strncpy(nums, token, sizeof(nums));
    nums[sizeof(nums) - 1] =
        '\0';  // Убедимся, что строка улицы завершена нулевым символом
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
  write(STDOUT_FILENO, &suma, sizeof(int));  // Записываем сумму в канал
}