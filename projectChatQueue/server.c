#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

/*********************************************************
**Программа-пользователь.                               **
**Программа получает доступ к очереди имён и очереди    **  
**сообщений,созданных программой-сервером. Программа    **
**позволяет пользователю отправлять сообщения и читать  **
**сообщения свои и других пользователей.                **
*********************************************************/

#define TEXT_SIZE 100
#define NAME_SIZE 10
#define NUM_SIZE 10

int msg_type = 1;

/* Структура, определяющая образец сообщения */
struct msg_buff{
    long type;  //тип сообщения 
    char text[TEXT_SIZE]; //сообщение
};

/* Функция отправки сообщений в очередь имён/сообщений */
void send_msg(int qid, char *buff){
    struct msg_buff snd_msg;

    //printf("Message identifier is %d\n", qid);
    strncpy(snd_msg.text, buff, TEXT_SIZE);
    snd_msg.type = msg_type;
    if(msgsnd(qid, (void *) &snd_msg, sizeof(snd_msg.text), IPC_NOWAIT) == -1){
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
    printf("sent: %s\n", snd_msg.text);
}

/* Функция чтения сообщений из очереди имён/сообщений */
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
