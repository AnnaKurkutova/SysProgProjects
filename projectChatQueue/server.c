#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "message.h"

/**********************************************************
**Программа-сервер.                                      **
**Программа создаёт очередь имён и очередь сообщений.    **
**Пользователь может читать все сообщения и имена других **
**пользователей.                                         **
**********************************************************/

/* Вывод возможных операций для пользователя */
void usage(){
    printf("Press 1 to read names\n");
    printf("Press 2 to read message\n");
    printf("Press 0 to exit\n");
}

void main(){
    int ch, rm;
    char *buff = malloc(NAME_SIZE);

    /* Создание очереди имён пользователей */
    key_t key_names = ftok("./server.c", 'a');
    int qid_names = msgget(key_names, IPC_CREAT | 0666);
    if(qid_names == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }
    /* Создание очереди сообщений пользователей*/
    key_t key_msgs = ftok("./server.c", 'b');
    int qid_msgs = msgget(key_msgs, IPC_CREAT | 0666);
    if(qid_msgs == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }
    
    int flag = 1; //для выхода из цикла
    usage();
    do{
        free(buff);
        buff = malloc(NUM_SIZE);
        fgets(buff, sizeof(buff), stdin);
        sscanf(buff, "%d", &ch);
        switch(ch){
        	/* чтение имён пользователей */
            case 1:{
                free(buff);
                buff = malloc(NAME_SIZE);
                receive_msg(qid_names, buff);
                send_msg(qid_names, buff);
            }
            break;
            /* чтение сообщений пользователей */
            case 2:{
                free(buff);
                buff = malloc(TEXT_SIZE);
                receive_msg(qid_msgs, buff);
                send_msg(qid_msgs, buff);
            }
            break;

            case 0:{
                flag = 0;
            }
            break;
        }
    }while(flag == 1);

    /* Удаление очереди имён пользователей */
    rm = msgctl(qid_names, IPC_RMID, NULL);
    if(rm){
        perror("msgctl error");
        exit(EXIT_FAILURE);
    }
    /* Удаление очереди сообщений */
    rm = msgctl(qid_msgs, IPC_RMID, NULL);
    if(rm){
        perror("msgctl error");
        exit(EXIT_FAILURE);
    }
    //освобождение памяти
    free(buff);
    exit(EXIT_SUCCESS);
}
