#include "diagonalMatrix.h"

int main(void){
  const int SIZE_OF_ARR = 10;
  int arr[SIZE_OF_ARR][SIZE_OF_ARR];
  
  createDiagonalMatrix(SIZE_OF_ARR, arr);
  printArr(SIZE_OF_ARR, arr);

  return(0);
}

