#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include "message.h"

/**********************************************************
**Программа-пользователь.                                **
**Программа получает доступ к разделяемой памяти  имён и **
**сообщений,созданных программой-сервером. Программа     **
**позволяет пользователю отправлять сообщения и читать   **
**сообщения свои и других пользователей.                 **
**********************************************************/

/* Функция ввода сообщения с клавиатуры */
void write_msg(char *buff){
    int ch;
    int i = 0;
    
    while (((ch = getchar()) != '\n') && (i < MAX_TEXT)) {
        buff[i++] = ch;
    }
    buff[i] = '\0';
}

/* Функция записи имени нового пользователя в разделяемую память,
перед запросом имени пользователю выводится список уже существующих 
в чате пользователей */
void set_name(message_t *name_p, char *buff, int semid){
    while(semctl(semid, 0, GETVAL, 0)); //если блокировка - ждать
    semctl(semid, 0, SETVAL, 1); //установить блокировку
    /* Чтение имён */
    printf("These users are already in chat:\n");
    while(name_p->type == MSG_TYPE_STRING){
        printf ("%s\n", name_p->string);
        name_p++;
    }
    /* Запись своего имени */
    printf("Enter your name: ");
    name_p->type = MSG_TYPE_STRING;
    write_msg(buff);
    strncpy(name_p->string, buff, MAX_TEXT);
    name_p++;
    semctl(semid, 0, SETVAL, 0); //отменить блокировку
}

/* Вывод возможных операций для пользователя */
void usage(){
    printf("Press 1 to send and read messages\n");
    printf("Press 0 to exit\n");
}

void main(){
    int ch;
    int shmid_names, shmid_msgs; //идентификаторы разделяемой памяти
    int semid; //идентификатор массива семафоров
    key_t key_names, key_msgs; //ключи разделяемой памяти
    int key_sem; //ключ массива семафоров
    message_t *msg_p; //адрес сообщения в разделяемой памяти
    message_t *name_p; //адрес имени в разделяемой памяти
    char *buff = malloc(MAX_NAME);

    /* Подключение к массиву семафоров из двух элементов */
    key_sem = ftok("./server.c", 'C');
    if((semid = semget(key_sem, 2, 0)) == -1)
    if(semid == -1){
        perror("semget error");
        exit(EXIT_FAILURE);
    }

    /* Создание разделяемой памяти имён пользователей */
    key_names = ftok("./server.c", 'A');
    shmid_names = shmget(key_names, sizeof(message_t)*NUM_MESSAGES, 0);
    if(shmid_names == -1){
        perror("shmget error");
        exit(EXIT_FAILURE);
    }

    /* Создание разделяемой памяти сообщений пользователей*/
    key_msgs = ftok("./server.c", 'B');
    shmid_msgs = shmget(key_msgs, sizeof(message_t)*NUM_MESSAGES, 0);
    if(shmid_msgs == -1){
        perror("shmget error");
        exit(EXIT_FAILURE);
    }
    
    /* Подключение сегмента к адресному пространству процесса */
    if ((msg_p = (message_t *) shmat(shmid_msgs, 0, 0)) == NULL){
        perror("shmat error");
        exit(EXIT_FAILURE);
    }

    /* Подключение сегмента к адресному пространству процесса */
    if ((name_p = (message_t *) shmat(shmid_names, 0, 0)) == NULL){
        perror("shmat error");
        exit(EXIT_FAILURE);
    }
    
    /* Пользователь в начале программы вводит своё имя */
    set_name(name_p, buff, semid);
 
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
                free(buff);
                buff = malloc(MAX_TEXT);
                while(semctl(semid, 1, GETVAL, 0)); //если блокировка - ждать
                semctl(semid, 1, SETVAL, 1); //установить блокировку
                /* Читаем все новые сообщения */
                while(msg_p->type == MSG_TYPE_STRING){
                    printf ("received: %s\n", msg_p->string);
                    msg_p++;
                }
                /* Пишем своё сообщение */
                printf("Enter: ");
                msg_p->type = MSG_TYPE_STRING;
                write_msg(buff);
                strncpy(msg_p->string, buff, MAX_TEXT);
                msg_p++;
                semctl(semid, 1, SETVAL, 0); //отменить блокировку
            }
            break;

            case 0:{
                flag = 0;
            }
            break;
        }
    }while(flag == 1);

    /* Отсоединение сегментов разделяемой памяти */
    shmdt(msg_p);
    shmdt(name_p);

    free(buff); //освобождение памяти
    exit(EXIT_SUCCESS);
}