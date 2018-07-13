#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

/**********************************************************
**Программа-сервер.                                      **
**Программа создаёт очередь имён и очередь сообщений.    **
**Пользователь может читать все сообщения и имена других **
**пользователей.                                         **
**********************************************************/
#define TEXT_SIZE 100 
#define NAME_SIZE 10
#define NUM_SIZE 10

int msg_type = 1;

/* Структура, определяющая образец сообщения */
struct msg_buff{
    long type;  //тип сообщения 
    char text[TEXT_SIZE]; //сообщение
};

/* Функция ввода сообщения с клавиатуры */
void write_msg(char *buff){
    int ch;
    int i = 0;
    
    while (((ch = getchar()) != '\n') && (i < TEXT_SIZE)) {
        buff[i++] = ch;
    }
    buff[i] = '\0';
}

/* Функция отправки сообщения в очередь сообщений*/
void send_msg(int qid, char *buff){
    struct msg_buff snd_msg;

    strncpy(snd_msg.text, buff, TEXT_SIZE);
    snd_msg.type = msg_type;
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

/* Функция отправки имени пользователя в очередь имён */
void enter_name(int qid, char *name){
    printf("Enter your name: \n");
    write_msg(name);
    send_msg(qid, name);
    receive_msg(qid, name);
    send_msg(qid, name);
}

/* Вывод возможных операций для пользователя */
void usage(){
    printf("Press 1 to read messages\n");
    printf("Press 2 to send message\n");
    printf("Press 0 to exit\n");
}

void main(){
    int ch, rm;
    char *buff = malloc(NAME_SIZE);

    /* Получение доступа к очереди имён пользователей */
    key_t key_names = ftok("./server.c", 'a');
    int qid_names = msgget(key_names, 0);
    if(qid_names == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }
    /* Получение доступа к очереди сообщений пользователей*/
    key_t key_msgs = ftok("./server.c", 'b');
    int qid_msgs = msgget(key_msgs, 0);
    if(qid_msgs == -1){
        perror("msgget error");
        exit(EXIT_FAILURE);
    }
    
    /* Пользователь в начале программы вводит своё имя */
    enter_name(qid_names, buff);
 
    int flag = 1; //для выхода из цикла
    usage();

    do{
        free(buff);
        buff = malloc(NUM_SIZE);
        fgets(buff, sizeof(buff), stdin);
        sscanf(buff, "%d", &ch);
        switch(ch){
        	/* Чтение сообщений */
            case 1:{
            	/* так как после чтения сообщения,
                оно стирается из очереди,
                обратно отправляем его в очередь */
                free(buff);
                buff = malloc(TEXT_SIZE);
                receive_msg(qid_msgs, buff);
                send_msg(qid_msgs, buff); 
            }
            break;
            
            /* Отправка сообщения */
            case 2:{
                free(buff);
                buff = malloc(TEXT_SIZE);
                printf("Enter: ");
                write_msg(buff);
                send_msg(qid_msgs, buff);
            }
            break;

            case 0:{
                flag = 0;
            }
            break;
        }
    }while(flag == 1);
    //освобождение памяти
    free(buff);
    exit(EXIT_SUCCESS);
}