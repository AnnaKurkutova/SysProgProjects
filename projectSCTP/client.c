#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/********************************************************************************
*  Данная программа является клиентом для протокола SCTP. Сначала создаётся     *
*  сокет SCTP, затем с помощью функции connect() осуществляется подключение к   *
*  серверу. Для работы по протоколу SCTP с клиентской стороны необходимо        *
*  настроить сокет. Это делается вызовом метода setsockopt(). Функция           *
*  setsockopt() вызывается со следующими параметрами: sfd - дескриптор          *
*  сокета, SOL_SCTP - уровень протокола SCTP, SCTP_EVENTS - параметр сокета,    *
*  который позволяет включать, выключать и определять состояние подписки на     *
*  различные уведомления SCTP, (const void *)&events - структура состояний и    *
*  sizeof(events) - размер структуры. Когда соединение установлено, клиент      *
*  ждёт сообщения от сервера. Для этого применяется метод sctp_recvmsg().       *
*  Функция sctp_recvmsg() вызывается со следующими параметрами: sfd -           *
*  дескриптор сокета, buff - массив под сообщение, sizeof(buff) - размер        *
*  массива, (struct sockaddr *)&my_addr - указатель на npoint отправителя,      *
*  &addr_size - размер структуры, в качестве остальных параметров передаётся    *
*  0. Результатом работы программы будет получение сообщения с текущим          *
*  временем от сервера.                                                         *
********************************************************************************/

#define BUFF_SIZE 50

/* Функция ввода сообщения с клавиатуры */
void write_msg(char *buff){
    int ch;
    int i = 0;
    
    while (((ch = getchar()) != '\n') && (i < BUFF_SIZE)) {
        buff[i++] = ch;
    }
    buff[i] = '\0';
}

void main(){
    int sfd; //файловый дескриптор
    struct sockaddr_in my_addr;
    struct sctp_event_subscribe events;
    socklen_t addr_size;
    char buff[BUFF_SIZE];

    /* Задаем адрес и порт */
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port =htons(7777);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    /* Инициализируем дескриптор сокета */
    if((sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Соединяемся с сервером */
    if(connect(sfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1) {
        perror("connection error");
        exit(EXIT_FAILURE);
    }

    /* Для того чтобы получить номер потока сообщений по протоколу SCTP, 
    необходимо указать параметр сокета sctp_data_io_event */
    memset((void *)&events, 0, sizeof(events));
    events.sctp_data_io_event = 1;
    setsockopt( sfd, SOL_SCTP, SCTP_EVENTS, (const void *)&events, sizeof(events));

    addr_size = sizeof(struct sockaddr_in);

    /* Принимаем сообщение от сервера */
    if(sctp_recvmsg(sfd, buff, BUFF_SIZE, (struct sockaddr *)&my_addr, &addr_size, 0, 0 ) == -1){
        close(sfd);
        perror("recv error");
        exit(EXIT_FAILURE);
    }
    printf("received from server: %s\n", buff);

    /* Закрываем дескриптор, связь моментально обрывается */
    close(sfd);
}