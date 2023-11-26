#include <iostream>
using namespace std;
#include "func.h"

int main() {
  int fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  ftruncate(fd, BUFFER_SIZE);

  char* shared_memory =
      (char*)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  pid_t child_pid = fork();

  if (child_pid == -1) {
    cerr << "Ошибка при создании дочернего процесса." << endl;
    return 1;
  }
  //////////////////////////////////////////////////////////////////////////////
  if (child_pid == 0) {  // Дочерний процесс

    if (execl("./child", "./child", NULL) == -1) {
      perror("Call execl was ended with erorr: ");
      exit(-1);
    }
    //////////////////////////////////////////////////////////////////////////////////////
  } else {
    // Родительский процесс

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

    strncpy(shared_memory, filename_nums, BUFFER_SIZE);
    wait(NULL);
    char suma[100];
    strncpy(suma, shared_memory, sizeof(suma));
    int summ = atoi(suma);
    cout << "Summa = " << summ << endl;
    close(fd);
    munmap(shared_memory, BUFFER_SIZE);
    shm_unlink(SHARED_MEMORY_NAME);
    wait(NULL);
  }
}