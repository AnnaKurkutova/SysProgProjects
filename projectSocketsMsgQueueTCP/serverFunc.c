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

/* Функция отправки сообщения в очередь сообщений*/
void send_msg(int qid, int cfd, char *buff){
    struct msg_buff snd_msg;

    strncpy(snd_msg.text, buff, WELCOME_MSG_SIZE);
    snd_msg.type = MSG_TYPE;
    snd_msg.sfd = cfd;

    if(msgsnd(qid, (void *) &snd_msg, sizeof(struct msg_buff) - sizeof(long), 0) == -1){
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
    printf("sent to queue: %s\n", snd_msg.text);
}

/* Функция получения сообщения из очереди сообщений*/
void receive_msg(int qid, int *cfd, char *buff){
    struct msg_buff rcv_msg;

    if(msgrcv(qid, (void *) &rcv_msg, sizeof(struct msg_buff) - sizeof(long), 0, 0) == -1){
        if(errno != ENOMSG){
            perror("msgrcv error");
            exit(EXIT_FAILURE);
        }
        printf("No message available for msgrcv()\n");
    }
    else{
        printf("received from queue: %s\n", rcv_msg.text);
    }

    strncpy(buff, rcv_msg.text, WELCOME_MSG_SIZE);
    *cfd = rcv_msg.sfd;
}

/* Функция потока */
void *workWithClient(void *arg){
    int num = *((int*)arg);
    int cfd; //дескриптор сокета
    struct sockaddr_in addr; //структура, хранящая npoint клиента/сервера
    int state = 0; //состояние
    int substate_first, substate_second, substate_third; //подсостояния
    socklen_t addr_size; //размер структуры
    char *message = malloc(BUFF_SIZE); //сообщение от клиента/сервера
    char welcome_msg = 'H';

    addr_size = sizeof(struct sockaddr_in);

    key_t key_msgs = ftok("./server.c", 'a'); //создание ключа очереди
    int qid_msgs = msgget(key_msgs, 0); //получение доступа к очереди сообщений пользователей
    if(qid_msgs == -1){ //проверка ошибок в присоединении к очереди сообщений
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

    printf("Thread number %d\n", num);

    while(1){
        switch(state){
            case CHECK://чтение сообщения от пользователя
                free(message);
                message = malloc(WELCOME_MSG_SIZE);
                receive_msg(qid_msgs, &cfd, message); //получаем сообщение и дескриптор для взаимодействия с клиентом из очереди
                substate_first = strncmp(message, &welcome_msg, WELCOME_MSG_SIZE); //проверяем корректность сообщения
                switch(substate_first){
                    case SUCCEED: //сообщение корректно, переходим к отправке сообщения клиенту
                        state = WRITE;
                        break;
                    
                    default: //сообщение некорректно, возвращаемся в начало
                        printf("Message is incorrect\n");
                }
                break;

            case WRITE: //отправка сообщения клиенту
                free(message);
                message = malloc(BUFF_SIZE);
                strncpy(message, "I'm your server", BUFF_SIZE);
                substate_second = send(cfd, message, BUFF_SIZE, 0);
                switch(substate_second){
                    case ERROR: //есть ошибки, возвращаемся в начало
                        close(cfd);
                        perror("sendto error");
                        state = CHECK;
                        break;

                    default: //нет ошибок, выводим сообщение, переходим к ожиданию сообщения от клиента
                        printf("sent to client: %s\n", message); 
                        state = READ;
                }
                break;
            
            case READ: //ожидание сообщения от клиента
                free(message);
                message = malloc(BUFF_SIZE);
                substate_third = recv(cfd, message, BUFF_SIZE, 0);
                switch(substate_third){
                    case ERROR: //есть ошибки, возвращаемся в начало
                        close(cfd);
                        perror("sendto error");
                        state = CHECK;
                        break;

                    default: //нет ошибок, выводим сообщение, возвращаемся в начало
                        printf("received from client: %s\n", message);
                        state = CHECK;
                }
                break;
        }
    }
    
    free(message);
    exit(EXIT_SUCCESS);
}
