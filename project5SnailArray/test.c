#include "snailArray.h"

int main(void){
  const int SIZE_OF_ARR = 5;
  int arr[SIZE_OF_ARR][SIZE_OF_ARR];
  
  createSnailArray(SIZE_OF_ARR, arr);
  printArr(SIZE_OF_ARR, arr);

  return(0);
}

