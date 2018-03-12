#include <stdio.h>

void printArr(int arr[], const int SIZE_OF_ARR){
  for(int i = 0; i < SIZE_OF_ARR; i++){
    printf("%d ", arr[i]);
  }
  printf("\n");
}

//Заполнение массива размера SIZE_OF_ARR числами, начиная с единицы
void fillArr(int arr[], const int SIZE_OF_ARR){
  for(int i = 0; i < SIZE_OF_ARR; i++){
    arr[i] = i + 1;
  }
}

//Правая и левая часть массива меняются местами
void bisectAndSwap(int arr[], const int SIZE_OF_ARR){
  const int HALF_SIZE = SIZE_OF_ARR / 2;
  int numOfChange = HALF_SIZE;
  int buf;

  /*Первый элемент левой части и 
  первый элемент правой части массива
  меняются и так далее*/
  for(int i = 0; i < HALF_SIZE; i++, numOfChange++){
    buf = arr[i];
    arr[i] = arr[numOfChange];
    arr[numOfChange] = buf;
  }
  //Проверка на чётность
  if(SIZE_OF_ARR != HALF_SIZE * 2){
    for(int i = HALF_SIZE; i < SIZE_OF_ARR; i++){
      buf = arr[i];
      arr[i] = arr[numOfChange];
      arr[numOfChange] = buf;
    }
  }
}




