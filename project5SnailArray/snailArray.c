#include <stdio.h>

void createSnailArray(const int SIZE_OF_ARR, int arr[][SIZE_OF_ARR]){
  int i = 0;
  int j = 0;
  int startPointNumber = 1;// первое число массива
  int count = SIZE_OF_ARR;// счётчик цикла
  int leastValue = 0;// наименьший номер элемента массива
  int highestValue = SIZE_OF_ARR;// наибольший номер элемента массива
  int modTwoSize = SIZE_OF_ARR % 2;// проверка на чётность размера массива

  while(count > 0){
    // заполнение идёт вправо
    if(count % 2 == modTwoSize){
      for(; j < highestValue; j++){
        arr[i][j] = startPointNumber++;
      }
      i++;// переход на следующую строку
      j--;// возврат к предыдущему столбцу
    }
    // заполнение идёт влево
    else {
      for(; j >= leastValue; j--){
        arr[i][j] = startPointNumber++;
      }
      leastValue++;
      i--;// возврат к предыдущей строке
      j++;// переход на следующий столбец
    }
    // заполнение идёт вниз
    if(count % 2 == modTwoSize){
      for(; i < highestValue; i++){
        arr[i][j] = startPointNumber++;
      }
      highestValue--;
      i--;// возврат к предыдущей строке
      j--;// возврат к предыдущему столбцу
    }
    // заполнение идёт вверх
    else {
      for(; i >= leastValue; i--){
        arr[i][j] = startPointNumber++;
      }
      i++;// переход на следующую строку
      j++;// переход на следующий столбец
    }
    count--;
  }
}

void printArr(const int SIZE_OF_ARR, int arr[][SIZE_OF_ARR]){
  printf("Матрица улитка размером %d : \n", SIZE_OF_ARR);
  for(int i = 0; i < SIZE_OF_ARR; i++){
    for(int j = 0; j < SIZE_OF_ARR; j++){
      /*ровный вывод, перед однозначными числами будет 0,
      например, 07 */
      printf("%d%d ", arr[i][j] / 10, arr[i][j] % 10);
    }
    printf("\n");
  }
  printf("\n");
}
