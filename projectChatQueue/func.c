#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "message.h"

/* Функция отправки сообщения в очередь сообщений*/
void send_msg(int qid, char *buff){
    struct msg_buff snd_msg;

    strncpy(snd_msg.text, buff, TEXT_SIZE);
    snd_msg.type = MSG_TYPE;
    if(msgsnd(qid, (void *) &snd_msg, sizeof(snd_msg.text), IPC_NOWAIT) == -1){
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
    printf("sent: %s\n", snd_msg.text);
}

/* Функция получения сообщения из очереди сообщений*/
void receive_msg(int qid, char *buff){
    struct msg_buff rcv_msg;

    if(msgrcv(qid, (void *) &rcv_msg, sizeof(rcv_msg.text), 0, IPC_NOWAIT) == -1){
        if(errno != ENOMSG){
            perror("msgrcv error");
            exit(EXIT_FAILURE);
        }
        printf("No message available for msgrcv()\n");
    }
    else{
        printf("received: %s\n", rcv_msg.text);
    }

    strncpy(buff, rcv_msg.text, NAME_SIZE);
}