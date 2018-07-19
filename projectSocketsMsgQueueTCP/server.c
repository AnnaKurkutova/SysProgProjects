#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stddef.h> 
#include <pthread.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "server.h"

/* ****************************************************************
**Программа является последовательным TCP сервером с очередью    ** 
**обслуживания. В программе создаются потоки, и каждый поток     **
**берёт из очереди, которую создал главный поток, символ и       **
**дескриптор для работы с клиентом и обслуживает его, а именно:  **
**отправляет клиенту сообщение "I'm your server" и ждёт от него  **
**сообщение с текущим временем.                                  **
******************************************************************/

void main(){
    int sfd, cfd; //дескрипторы сокетов
    int ch, rm;
    int port_num = 7777;
    int result;
    int num[THREADS_NUM]; //идентификаторы потоков
    pthread_attr_t attr; //атрибут для создания потока
    pthread_t threads[THREADS_NUM]; //потоки
    struct sockaddr_in my_addr, peer_adr; //структуры, хранящие npoint клиента/сервера
    socklen_t peer_addr_size; //размер структуры
    void *status; //статус завершения потока
    char buff[WELCOME_MSG_SIZE]; //строка с сообщением

    key_t key_msgs = ftok("./server.c", 'a'); //создание ключа для очереди
    int qid_msgs = msgget(key_msgs, IPC_CREAT | 0666); //создание очереди сообщений пользователей
    
    if(qid_msgs == -1){ //проверка ошибок в создании очереди
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

    memset(&my_addr, 0, sizeof(struct sockaddr_in)); //зануляем структуру
    my_addr.sin_family = AF_INET; //задаём семейство
    my_addr.sin_port = htons(port_num); //задаём порт
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //задаём адрес прослушивания

    if((sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){ //инициализируем дескриптор сокета
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    if((bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in))) == -1){ //связываем адрес с дескриптором слушающего сокета
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if(listen(sfd, LISTEN_NUM) == -1){ //начинаем слушать сокет
        close(sfd);
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    peer_addr_size = sizeof(struct sockaddr_in);

    pthread_attr_init(&attr); //инициализация атрибута
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); //установка атрибута

    for(int i = 0; i < THREADS_NUM; i++){ //заполнение массива идентификаторов потоков
        num[i] = i;
    }

    for(int i = 0; i < THREADS_NUM; i++){ //создание потоков
        result = pthread_create(&threads[i], &attr, workWithClient, (void*) &num[i]); 
        errorCreateThread(result); //контроль ошибок
    }

    while(1){
        if((cfd = accept(sfd, (struct sockaddr *) &peer_adr, &peer_addr_size)) == -1){ //принимаем соединение, создаём дескриптор для взаимодействия с конкретным клиентом
            close(sfd);
            perror("accept error");
            exit(EXIT_FAILURE);
        }

        if(recv(cfd, buff, WELCOME_MSG_SIZE, 0) == -1){ //принимаем сообщение от клиента
            close(cfd);
            perror("recv error");
            exit(EXIT_FAILURE);
        }
        printf("received from client: %s\n", buff);
    
        send_msg(qid_msgs, cfd, buff); //отправляем в очередь дескриптор для взаимодействия с конкретным клиентом и сообщение клиента
    }

    for(int i = 0; i < THREADS_NUM; i++){ //ожидание завершения потоков
        pthread_join(threads[i], &status); 
        errorJoinThread(status); //контроль ошибок
    }
    
    pthread_attr_destroy(&attr);//уничтожение атрибута

    rm = msgctl(qid_msgs, IPC_RMID, NULL); //удаление очереди сообщений
    if(rm){ //проверка ошибок в удалении очереди сообщений
        perror("msgctl error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS); //завершение программы
}