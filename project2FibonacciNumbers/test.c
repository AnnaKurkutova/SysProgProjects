#include "sequenceFibonacci.h"

int main(void){
  const int NUM = 20;

  int *ptr = createSequenceFibonacci(NUM);
  printSequenceFibonacci(ptr, NUM);

  return(0);
}

