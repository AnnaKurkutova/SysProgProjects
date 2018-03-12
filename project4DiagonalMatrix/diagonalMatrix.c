#include <stdio.h>

void createDiagonalMatrix(const int SIZE_OF_ARR, int arr[][SIZE_OF_ARR]){
  int diagNum = SIZE_OF_ARR - 1;//номер столбца, с которого начинаются единицы
  
  for(int i = 0; i < SIZE_OF_ARR; i++){
    for(int j = 0; j < SIZE_OF_ARR; j++){
      if(j < diagNum){
        arr[i][j] = 0;
      }
      else {
        arr[i][j] = 1;
      }
    }
    diagNum--;
  }
}

void printArr(const int SIZE_OF_ARR, int arr[][SIZE_OF_ARR]){
  printf("Диагональная матрица размером %d : \n", SIZE_OF_ARR);
  for(int i = 0; i < SIZE_OF_ARR; i++){
    for(int j = 0; j < SIZE_OF_ARR; j++){
      printf("%d ", arr[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}
