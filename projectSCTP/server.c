#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <errno.h>
#include <string.h>
#include <stddef.h> 
#include <time.h>

/********************************************************************************
*  Данная программа является сервером для протокола SCTP. Сначала создаётся     *
*  сокет SCTP затем с помощью функции sctp_bindx() адрес связывается с          *
*  дескриптором слушающего сокета. Метод sctp_bindx() вызывается со             *
*  следующими параметрами: sfd - дескриптор сокета, (struct sockaddr *)         *
*  &my_addr - массив IP-адресов, 1 - количество элементов в массиве,            *
*  SCTP_BINDX_ADD_ADDR - флаг добавления адреса связывания. Затем сокет         *
*  сервера переводится в состояние ожидания соединения с помощью функции        *
*  listen(). Функция accept()позволяет устанавливать соединение с новым         *
*  клиентом. Когда соединение установлено, клиенту отправляется сообщение       *
*  вызовом функции sctp_sendmsg(). Метод sctp_sendmsg() выывается со            *
*  следующими параметрами:cfd - дескриптор для взаимодействия с конкретным      *
*  клиентом, buff - сообщение, BUFF_SIZE - размер сообщения, (struct sockaddr   *
*  *) &peer_adr - адрес назначения, peer_addr_size - размер адреса, в           *
*  качестве остальных параметров, в том числе номера потока, передаётся 0.      *
*  Результатом работы данной программы будет сообщение с текущим временем,      *
*  отправленное клиенту.                                                        *
********************************************************************************/

#define LISTEN_NUM 5
#define BUFF_SIZE 100

void main(){
    long int my_time; //переменная для сохранения текущего времени
    int sfd, cfd; //файловые дескрипторы
    struct sockaddr_in my_addr, peer_adr;
    socklen_t peer_addr_size;
    char buff[BUFF_SIZE];

    /* Задаем адрес прослушивания и порт */
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(7777);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Инициализируем дескриптор сокета */
    if((sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Связываем адрес с дескриптором слушающего сокета */
    if((sctp_bindx(sfd, (struct sockaddr *) &my_addr, 1, SCTP_BINDX_ADD_ADDR)) == -1){
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    /* Начинаем слушать сокет */
    if(listen(sfd, LISTEN_NUM) == -1){
        close(sfd);
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    peer_addr_size = sizeof(struct sockaddr_in);

    /* Принимаем соединение, создаём дескриптор для взаимодействия с конкретным клиентом */
    if((cfd = accept(sfd, (struct sockaddr *) &peer_adr, &peer_addr_size)) == -1){
        close(sfd);
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    
    /* Считываем текущее время */
    my_time = time (NULL);
    strncpy(buff, ctime(&my_time), BUFF_SIZE);

    /* Посылаем текущее время клиенту по потоку 0 */
    if(sctp_sendmsg(cfd, buff, BUFF_SIZE, (struct sockaddr *) &peer_adr, peer_addr_size, 0, 0, 0, 0, 0 ) == -1){
        close(cfd);
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }
    printf("sent to client: %s\n", buff);

    /* Закрываем дескрипторы, соединение моментально обрывается */
    close(cfd);
    close(sfd);
}