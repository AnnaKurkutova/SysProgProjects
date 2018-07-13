#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "message.h"

/***************************************************************
**Программа-сервер.                                           **
**Программа создаёт разделяемую память имён и сообщений.      **
**Сервер может читать сообщения и имена других пользователей. **
***************************************************************/

/* Вывод возможных операций для пользователя */
void usage(){
    printf("Press 1 to read names\n");
    printf("Press 2 to read message\n");
    printf("Press 0 to exit\n");
}

void main(){
    int shmid_names, shmid_msgs; //идентификаторы разделяемой памяти
    int semid; //идентификатор массива семафоров
    int ch;
    key_t key_names, key_msgs; //ключи разделяемой памяти
    key_t key_sem; //ключ массива семафоров
    message_t *msg_p; //адрес сообщения в разделяемой памяти
    message_t *name_p; //адрес имени в разделяемой памяти
    char *buff;

    /* Cоздание массива семафоров из двух элементов */
    key_sem = ftok("./server.c", 'C');
    if((semid = semget(key_sem, 2, IPC_CREAT | 0666)) == -1)
    if(semid == -1){
        perror("semget error");
        exit(EXIT_FAILURE);
    }

    /* Создание разделяемой памяти имён пользователей */
    key_names = ftok("./server.c", 'A');
    shmid_names = shmget(key_names, sizeof(message_t)*NUM_MESSAGES, IPC_CREAT | 0666);
    if(shmid_names == -1){
        perror("shmget error");
        exit(EXIT_FAILURE);
    }
    /* Создание разделяемой памяти сообщений пользователей*/
    key_msgs = ftok("./server.c", 'B');
    shmid_msgs = shmget(key_msgs, sizeof(message_t)*NUM_MESSAGES, IPC_CREAT | 0666);
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

    /* Установка семафоров */
    semctl(semid, 0, SETVAL, 0);
    semctl(semid, 1, SETVAL, 0);

    int flag = 1; //для выхода из цикла
    usage();
    do{
        buff = malloc(NUM_SIZE);
        fgets(buff, sizeof(buff), stdin);
        sscanf(buff, "%d", &ch);
        switch(ch){
            /* Чтение имён пользователей */
            case 1:{
                while(semctl(semid, 0, GETVAL, 0)); //если блокировка - ждать
                semctl(semid, 0, SETVAL, 1); //установить блокировку
                /* Чтение имён */
                while(name_p->type == MSG_TYPE_STRING){
                    printf ("user: %s\n", name_p->string);
                    name_p++;
                }
                printf("No more users in chat\n");
                semctl(semid, 0, SETVAL, 0); //снять блокировку
            }
            break;
            /* Чтение сообщений пользователей */
            case 2:{
                while(semctl(semid, 1, GETVAL, 0)); //если блокировка - ждать 
                semctl(semid, 1, SETVAL, 1); //установить блокировку
                /* Чтение сообщений */
                while(msg_p->type == MSG_TYPE_STRING){
                    printf ("received: %s\n", msg_p->string);
                    msg_p++;
                }
                printf("No more messages in chat\n");
                semctl(semid, 1, SETVAL, 0); //снять блокировку
            }
            break;

            case 0:{
                flag = 0;
            }
            break;
        }
        free(buff);
    }while(flag == 1);


    /* Удаление массива семафоров */
    if (semctl(semid, 0, IPC_RMID, (struct semid_ds *) 0) == -1){
        perror("semctl error");
        exit(EXIT_FAILURE);
    }

    /* Удаление сегментов разделяемой памяти */
    shmdt(msg_p);
    if (shmctl(shmid_msgs, IPC_RMID, (struct shmid_ds *) 0) == -1){
        perror("shmctl error");
        exit(EXIT_FAILURE);
    }

    shmdt(name_p);
    if (shmctl(shmid_names, IPC_RMID, (struct shmid_ds *) 0) == -1){
        perror("shmctl error");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
