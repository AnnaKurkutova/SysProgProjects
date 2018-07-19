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
#include <time.h>
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
void send_msg(int qid, int cfd, char buff, struct sockaddr_in addr){
    struct msg_buff snd_msg;

    snd_msg.sfd = cfd;
    snd_msg.type = MSG_TYPE;
    snd_msg.address.sin_family = addr.sin_family;
    snd_msg.address.sin_port = addr.sin_port;
    snd_msg.address.sin_addr.s_addr = addr.sin_addr.s_addr;
    snd_msg.sym = buff;

    if(msgsnd(qid, (void *) &snd_msg, sizeof(struct msg_buff) - sizeof(long), 0) == -1){
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
    printf("sent to queue: %c\n", snd_msg.sym);
}

/* Функция получения сообщения из очереди сообщений*/
void receive_msg(int qid, int *cfd, char *buff, struct sockaddr_in *addr){
    struct msg_buff rcv_msg;

    if(msgrcv(qid, (void *) &rcv_msg, sizeof(struct msg_buff) - sizeof(long), 0, 0) == -1){
        if(errno != ENOMSG){
            perror("msgrcv error");
            exit(EXIT_FAILURE);
        }
        printf("No message available for msgrcv()\n");
    }
    else{
        printf("received from queue: %c\n", rcv_msg.sym);
    }

    *cfd = rcv_msg.sfd;
    addr->sin_family = rcv_msg.address.sin_family;
    addr->sin_port = rcv_msg.address.sin_port;
    addr->sin_addr.s_addr = rcv_msg.address.sin_addr.s_addr;
    *buff = rcv_msg.sym;
}

/* Функция потока */
void *workWithClient(void *arg){
    long int my_time; //переменная для сохранения текущего времени
    int num = *((int*)arg);
    int cfd; //дескриптор сокета
    int state = CHECK; //состояние
    int substate_first, substate_second; //подсостояния
    struct sockaddr_in addr; //npoint клиента
    socklen_t addr_size; //размер структуры
    char *message = malloc(BUFF_SIZE);
    char buff;
    char welcome_msg = 'H';

    addr_size = sizeof(struct sockaddr_in);

    key_t key_msgs = ftok("./server.c", 'a'); //создание ключа для подключения к очереди
    int qid_msgs = msgget(key_msgs, 0); //получение доступа к очереди сообщений пользователей
    if(qid_msgs == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

    printf("Thread number %d\n", num);

    while(1){
        switch(state){
            case CHECK://чтение сообщения от пользователя
                receive_msg(qid_msgs, &cfd, &buff, &addr); //получаем сообщение, дескриптор и npoint клиента из очереди
                substate_first = strncmp(&buff, &welcome_msg, 1);
                switch(substate_first){
                    case SUCCEED:
                        state = WRITE;
                        break;

                    default:
                        printf("Incorrect message\n");
                }

                break;

            case WRITE: //отправка сообщения клиенту
                free(message);
                message = malloc(BUFF_SIZE);
                strncpy(message, "I'm your server", BUFF_SIZE);
                my_time = time (NULL); //считываем текущее время
                strncpy(message, ctime(&my_time), BUFF_SIZE); //считанное время  переводим в локальное, а затем в строку
                substate_second = sendto(cfd, message, BUFF_SIZE, 0, (struct sockaddr *) &addr, addr_size);
                switch(substate_second){
                    case ERROR: //есть ошибки, возвращаемся в начало
                        close(cfd);
                        perror("sendto error");
                        state = CHECK;
                        break;

                    default: //нет ошибок, выводим сообщение, переходим к ожиданию сообщения от клиента
                        printf("sent to client: %s\n", message); 
                        state = CHECK;
                }
                break;

        }
    }

    free(message);
    exit(EXIT_SUCCESS);
}
