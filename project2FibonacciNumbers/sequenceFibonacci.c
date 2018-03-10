#include <stdio.h>

int* createSequenceFibonacci(const int SIZE){
  const int SIZE_OF_ARR = SIZE + 2;
  int arrOfNum[SIZE_OF_ARR];

  arrOfNum[0] = 1;
  arrOfNum[1] = 1;
  for(int i = 2; i < SIZE_OF_ARR; i++){
    arrOfNum[i] = arrOfNum[i-2] + arrOfNum[i-1];
  }
  int *ptr = arrOfNum;

  return ptr; 
}

int printSequenceFibonacci(int* ptr, const int SIZE){
  const int SIZE_OF_ARR = SIZE + 2;

  printf("Последовательность Фибоначчи из %d чисел:\n", SIZE_OF_ARR);
  for(int i = 0; i < SIZE_OF_ARR; i++){
    printf("%d ", ptr[i]);
  }
  printf("\n");
  
  return(0);
}
