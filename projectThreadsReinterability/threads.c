#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
 
/*************************************************************
**Данная программа создаёт 5 потоков и для каждого потока   **
**инициализируется локальная память. Локальная память       **
**содержит структуру данных data_t(идентификатор потока +   **
**его порядковый номер). Таким образом, все потоки,         **
**обращаясь к одним и тем же переменным получает свои       **
**собственные переменные - идентификатор и порядковый номер.**
**************************************************************/

#define COUNT 5
#define BUF_SIZE 9

pthread_key_t key;
pthread_once_t once = PTHREAD_ONCE_INIT;

/* Структура, которая хранит идентификатор потока tid и его порядковый номер */
typedef struct data_t{
   pthread_t tid;
   char *threadNum;
} data_t;
 
/* Деструктор данных */
void destructor(void* arg){
   data_t *ptr = (data_t*)arg;
   free(ptr);
}

/* Регистрация ключа */
void initKey(){
   pthread_key_create(&key, destructor);
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
}

/* Функция контроля ошибок при фиксировании значений локальной памяти потока */
void errorSetspecific(int status){
	if(status != 0){
        perror("Error in setspecific");
        exit(EXIT_FAILURE);
    }
}

/* Функция контроля ошибок при получении текущего значения локальной памяти потока */
void errorGetspecific(data_t *data){
    if(data == NULL){
        perror("Error in getspecific");
        exit(EXIT_FAILURE);
    }
}

/* Функция потока */
void* errno(void *arg){
    char *buff = (char*)arg; //порядковый номер потока
    int status;

    pthread_once(&once, initKey); //гарантия уникальности создания ключа
   
    status = pthread_setspecific(key, malloc(sizeof(data_t))); //фиксирование значений локальной памяти потока
    errorSetspecific(status);
    data_t *db = pthread_getspecific(key); //получение текущего значения локальной памяти потока
    errorGetspecific(db);
    db->tid = pthread_self(); //в структуру помещается tid потока
    db->threadNum = buff; //в структуру помещается порядковый номер потока

    //выод на экран локальных данных потока
    printf( "%s, tid = %lu\n", ((data_t*)pthread_getspecific(key))->threadNum, ((data_t*)pthread_getspecific(key))->tid);
    pthread_exit(NULL);  
}
 
void main(){
	int i, j, result; //счётчик цикла и результат создания потока
    pthread_attr_t attr; //атрибут для создания потока
	pthread_t threads[COUNT]; //потоки-покупатели
	char num[COUNT][BUF_SIZE]; //порядковые номера потоков
    void *status; //статус завершения потока
    
    /* Инициализация и установка атрибута */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* Заполнение массива идентификаторов потоков */
    strncpy(num[0], "Thread 1", BUF_SIZE);
    strncpy(num[1], "Thread 2", BUF_SIZE);
    strncpy(num[2], "Thread 3", BUF_SIZE);
    strncpy(num[3], "Thread 4", BUF_SIZE);
    strncpy(num[4], "Thread 5", BUF_SIZE);

    for(i = 0; i < COUNT; i++){
        result = pthread_create(&threads[i], &attr, errno, (void*) &num[i]); //создание потока
        errorCreateThread(result); //контроль ошибок
    }
    
    /* Главный поток ждёт завершения остальных потоков */
    for(i = 0; i < COUNT; i++){
        pthread_join(threads[i], &status); 
        errorJoinThread(status); //контроль ошибок
    }
    printf("Все потоки завершились\n");
    
    pthread_attr_destroy(&attr);

    exit(EXIT_SUCCESS);
}
