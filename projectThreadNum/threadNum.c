#include <stdlib.h>
#include <pthread.h>
#include "threadNum.h"

#define COUNT 5

int main(){
    int i, result; //счётчик цикла и результат создания потока
    pthread_attr_t attr; //атрибут для создания потока
	pthread_t threads[COUNT]; //потоки
    int num[COUNT]; //идентификаторы потоков
    void *status; //статус завершения потока
    
    // инициализация и установка атрибута
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //заполнение массива идентификаторов потоков
    for(i = 0; i < COUNT; i++){
        num[i] = i + 1;
    }
    
    for(i = 0; i < COUNT; i++){
        result = pthread_create(&threads[i], &attr, printNum, (void*) &num[i]); //создание потока
        errorCreateThread(result); //контроль ошибок
    }
    
    //ожидание завершения потоков
    for(i = 0; i < COUNT; i++){
        pthread_join(threads[i], &status); 
        errorJoinThread(status); //контроль ошибок
    }

    pthread_attr_destroy(&attr);
    exit(EXIT_SUCCESS);
}
