#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

/********************************************************************************************
**Данная программа создаёт 3 потока-покупателя и один поток-погрузчик.                     **
**Существует 5 магазинов - массив int store[STORE_COUNT], где store[i]- это                **
**количество товаров в магазине. За один заход в цикл ("проход по магазинам")              ** 
**поток-покупатель покупает n товаров.У каждого потока-покупателя есть                     **
**запрос на товар, равный 10000, когда поток его исчерпал, он завершается.                 **
**Поток-погрузчик за один заход в цикл доставляет в один из магазинов 500 штук             **
**товара. В одном магазине store[i] может находиться одновременно только один              **
**поток. Для этого существует массив статусов магазинов storeStat[STORE_COUNT].            **
**Поток, заходя в магазин, блокирует его на время, необходимое ему для совершения          **
**действий с этим магазином. По завершении этих действий, магазин разблокируется.          **
**Когда поток-покупатель завершается, он сообщает об этом главному потоку                  **
**посредством условной переменной cond, и главный поток его дожидается, проверяя           **
**статус потока-покупателя с помощью  массива статусов покупателей buyerStat[BUYERS_COUNT].**
**Поток-погрузчик завершается главным потоком, когда все потоки-покупатели завершены.      **
********************************************************************************************/

#define STORE_COUNT 5
#define BUYERS_COUNT 3

int store[STORE_COUNT] = {1000, 700, 1200, 500, 1500}; //магазины
int storeStat[STORE_COUNT] = {0, 0, 0, 0, 0}; //статус магазинов, 0 - свободен, 1 - занят
int buyerStat[BUYERS_COUNT] = {0, 0, 0}; //статус завершения потоков-покупателей, 0 -не завершён, 1 - завершён
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //инициализация мьютекса
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //инициализация условия;

/* Функция потока-покупателя */
void *doShopping(void *arg){
    int num = *((int*)arg);
    int productDemand = 10000;

    printf("Покупатель %d пошёл на шоппинг\n", num);

    do{
        for(int i = 0; i < STORE_COUNT; i++){
            if((storeStat[i] == 0) && (store[i] > 0)){
                pthread_mutex_lock(&mutex); //захват мьютекса
                storeStat[i] = 1;
                pthread_mutex_unlock(&mutex); //освобождение мьютекса
                if(store[i] <= productDemand){
                    productDemand -= store[i];
                    printf("Покупатель %d купил в магазине номер %d %d штук товара\n", num, i, store[i]);
                    store[i] = 0;
                    storeStat[i] = 0;
                }
                else{
                    store[i] -= productDemand;
                    printf("Покупатель %d купил в магазине номер %d %d штук товара\n", num, i, productDemand);
                    productDemand = 0;
                }
                pthread_mutex_lock(&mutex); //захват мьютекса
                storeStat[i] = 0;
                pthread_mutex_unlock(&mutex); //освобождение мьютекса
                if(productDemand == 0){
                    buyerStat[num] = 1;
                    pthread_cond_signal(&cond);
                    pthread_exit(NULL);
                }
                else{
                    sleep(1);
                }    
            }
        }
    }while(buyerStat[num] = 1);
}

/* Функция потока-погрузчика */
void *deliverGoods(void *arg){
    int num = *((int*)arg);
    int goodsDelivered = 500;

    printf("Погрузчик пришёл на работу\n");

    while(1){
        for(int i = 0; i < STORE_COUNT; i++){
            if(storeStat[i] == 0){
                pthread_mutex_lock(&mutex); //захват мьютекса
                storeStat[i] = 1;
                pthread_mutex_unlock(&mutex); //освобождение мьютекса
                store[i] += 500;
                printf("Погрузчик доставил в магазин номер %d %d штук товара\n", i, goodsDelivered);
                pthread_mutex_lock(&mutex); //захват мьютекса
                storeStat[i] = 0;
                pthread_mutex_unlock(&mutex); //освобождение мьютекса
                sleep(2);
            } 
        } 
    }     
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

int main(){
    int i, result; //счётчик цикла и результат создания потока
    int num[BUYERS_COUNT]; //идентификаторы потоков
    int dNum = BUYERS_COUNT; //идентификатор потока-погрузчика
    pthread_attr_t attr; //атрибут для создания потока
    pthread_t deliveryman; //поток-погрузчик
    pthread_t buyers[BUYERS_COUNT]; //потоки-покупатели
    void *status; //статус завершения потока
    
    // инициализация и установка атрибута
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //заполнение массива идентификаторов потоков
    for(i = 0; i < BUYERS_COUNT; i++){
        num[i] = i;
    }

    for(i = 0; i < BUYERS_COUNT; i++){
        result = pthread_create(&buyers[i], &attr, doShopping, (void*) &num[i]); //создание потока
        errorCreateThread(result); //контроль ошибок
    }
    result = pthread_create(&deliveryman, &attr, deliverGoods, (void*) &dNum); //создание потока
    errorCreateThread(result); //контроль ошибок

    do{
        for(i = 0; i < BUYERS_COUNT; i++){
            pthread_join(buyers[i], &status); 
            errorJoinThread(status); //контроль ошибок
            printf("Покупатель %d купил всё, что хотел и пошёл домой\n", i); 
            sleep(1);
        }
    }while((buyerStat[0]+buyerStat[1]+buyerStat[2]+buyerStat[3]+buyerStat[4]) == 5);
    
    printf("Все %d покупателя ушли домой\n", BUYERS_COUNT);
    pthread_cancel(deliveryman);
    printf("Магазин закрылся, погрузчик больше сегодня не работает\n");

    pthread_mutex_destroy(&mutex); //уничтожение мьютекса
    pthread_cond_destroy(&cond); //уничтожение условия
    pthread_attr_destroy(&attr);

    exit(EXIT_SUCCESS);
}