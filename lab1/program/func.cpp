#include "func.h"

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