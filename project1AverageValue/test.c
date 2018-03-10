#include <stdio.h>
#include "averageValue.h"

int main(void){
  int firstValue = 6;
  int secondValue = 28;
  int averageValue;

  averageValue = computeAverage(firstValue, secondValue);
  printf("Среднее значение между %d и %d равно %d.\n", firstValue, secondValue, averageValue);
  return(0);
}

