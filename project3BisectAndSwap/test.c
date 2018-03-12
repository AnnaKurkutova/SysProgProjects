#include "bisectAndSwap.h"

int main(void){
  const int SIZE_OF_ARR = 24;
  int arr[SIZE_OF_ARR];

  fillArr(arr,SIZE_OF_ARR);
  printArr(arr, SIZE_OF_ARR);
  bisectAndSwap(arr, SIZE_OF_ARR);
  printArr(arr, SIZE_OF_ARR);

  return(0);
}
