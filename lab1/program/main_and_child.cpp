#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

using namespace std;

int sum_numbers(char input_nums[100]) {
  int sum = 0;
  int numbers[100];
  int count = 0;
  char* token = strtok(input_nums, " ");
  while (token != NULL && count < 100) {
    numbers[count] = atoi(token);  // Convert string to integer
    count++;
    token = strtok(NULL, " ");
  }
  for (int num = 0; num < count; num++) {
    sum += numbers[num];
  }
  return sum;
}

int main() {
  int pipe1[2], pipe2[2];  // Дескрипторы для канала

  // Создание каналов
  if (pipe(pipe1) == -1) {
    cerr << "Ошибка при создании канала." << endl;
    return 1;
  }
  if (pipe(pipe2) == -1) {
    cerr << "Ошибка при создании канала." << endl;
    return 1;
  }
  //////////////////////////////////////////////////////////////////////
  pid_t child_pid = fork();  // Создаем дочерний процесс

  if (child_pid == -1) {
    cerr << "Ошибка при создании дочернего процесса." << endl;
    return 1;
  }

  if (child_pid == 0) {  // Дочерний процесс
    close(pipe1[1]);  // Закрываем записывающий конец канала
    close(pipe2[0]);

    char fname_nums[200];
    char fname[100];
    char nums[100];
    read(pipe1[0], fname_nums, sizeof(fname_nums));
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

    write(pipe2[0], &suma, sizeof(int));  // Записываем сумму в канал
    printf("Сумма чисел: %d\n", suma);

    close(pipe1[0]);
    close(pipe2[1]);  // Закрываем читающий конец канала
  } else {
    /////////////////////////////////////////////////////////////////////////
    // Родительский процесс
    close(pipe1[0]);  // Закрываем читающий конец 1 канала
    close(pipe2[1]);  // Закрываем записывающий конец 2 канала
    char filename_nums[100];
    char filename[40];
    printf("Enter filename:\n");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    char input_nums[200];
    printf("Enter numbers:\n");
    fgets(input_nums, sizeof(input_nums), stdin);
    strcpy(filename_nums, filename);
    strcat(filename_nums, "\n");
    strcat(filename_nums, input_nums);

    write(pipe1[1], filename_nums,
          strlen(filename_nums) + 1);  // Записываем имя файла в канал

    close(pipe1[1]);
    close(pipe2[0]);  // Закрываем записывающий конец канала

    wait(NULL);  // Ждем завершения дочернего процесса
  }

  return 0;
}
