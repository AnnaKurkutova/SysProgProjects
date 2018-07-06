#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

/* Функция выводит идентификатор потока и завершает его */
void *printNum(void *arg){
	int num = *((int*)arg);
    printf("The %d thread was started\n", num);
    pthread_exit(NULL);
}

/* Функция контроля ошибок при создании потока */
void errorCreateThread(int result){
    if(result != 0){
        perror("Error in creating the thread");
        exit(EXIT_FAILURE);
    }
}

/* Функция контроля ошибок при ожидании завершения потока */
void errorJoinThread(void *status){
    if(status != 0){
        perror("Error in joining the thread");
        exit(EXIT_FAILURE);
    }
    printf("Successfully joined \n");
}