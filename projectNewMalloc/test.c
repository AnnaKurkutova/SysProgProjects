#include <stdio.h>
#include "myMalloc.h"

/* Тестовая программа, демонстрирующая работу с собственной динамической библиотекой,
в которой реализованы функции my_malloc(size_t) и my_free(void*), позволяющие осуществлять
динамическое выделение и освобождение памяти и схожие по функционалу с методами
malloc(size_t, const void*) free(void*, const void*) из библиотеки glibc.*/

int main(){
    int *firstInt = my_malloc(10*sizeof(int));
    for(int i = 0; i < 10; i++){
        firstInt[i] = i;
        printf("%d\n", firstInt[i]);
    }
    my_free(firstInt);

    int *secondInt = my_malloc(10*sizeof(int));
        for(int i = 0; i < 10; i++){
        secondInt[i] = i + 10;
        printf("%d\n", secondInt[i]);
    }

    char *firstChar = my_malloc(5*sizeof(char));
    my_free(firstChar);
    my_free(secondInt);
    return 0;
}