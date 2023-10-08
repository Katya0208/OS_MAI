#include <iostream>
using namespace std;
#include "func.h"

int main() {
  int pipe1[2], pipe2[2];
  if (pipe(pipe1) == -1) {
    cerr << "Ошибка при создании канала." << endl;
    return 1;
  }
  if (pipe(pipe2) == -1) {
    cerr << "Ошибка при создании канала." << endl;
    return 1;
  }

  pid_t child_pid = fork();

  if (child_pid == -1) {
    cerr << "Ошибка при создании дочернего процесса." << endl;
    return 1;
  }
  //////////////////////////////////////////////////////////////////////////////
  if (child_pid == 0) {  // Дочерний процесс
    close(pipe1[1]);  // Закрываем записывающий конец канала
    close(pipe2[0]);
    if (dup2(pipe1[0], STDIN_FILENO) == -1) {
      perror("Call dup2 was ended with erorr: ");
      exit(-1);
    }

    if (dup2(pipe2[1], STDOUT_FILENO) == -1) {
      perror("dup2 out ");
      exit(-1);
    }

    if (execl("./child", "./child", NULL) == -1) {
      perror("Call execl was ended with erorr: ");
      exit(-1);
    }
    close(pipe1[0]);
    close(pipe2[1]);
    //////////////////////////////////////////////////////////////////////////////////////
  } else {
    // Родительский процесс
    close(pipe1[0]);  // Закрываем читающий конец 1 канала
    close(pipe2[1]);

    char filename_nums[100];
    char filename[40];
    printf("Enter filename:\n");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    char input_nums[200];
    int suma = 0;
    printf("Enter numbers:\n");
    fgets(input_nums, sizeof(input_nums), stdin);
    strcpy(filename_nums, filename);
    strcat(filename_nums, "\n");
    strcat(filename_nums, input_nums);

    write(pipe1[1], filename_nums,
          strlen(filename_nums) + 1);  // Записываем имя файла в канал

    read(pipe2[0], &suma, sizeof(int));
    printf("Сумма чисел: %d\n", suma);

    close(pipe1[1]);
    close(pipe2[0]);  // Закрываем записывающий конец канала

    wait(NULL);
  }
}