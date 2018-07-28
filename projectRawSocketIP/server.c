#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stddef.h> 

#define LISTEN_NUM 5
#define MSG_SIZE 150
#define SOURCE_PORT 7777

void main(){
    int sfd; //файловый дескриптор
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buff[MSG_SIZE];

    /* Задаем адрес и порт */
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOURCE_PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    /* Инициализируем дескриптор сокета */
    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Связываем адрес с дескриптором сокета */
    if((bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in))) == -1){
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(struct sockaddr_in);

    /* Принимаем сообщение от клиента */
    if(recvfrom(sfd, buff, MSG_SIZE, 0, (struct sockaddr *) &addr, &addr_size) == -1){
        close(sfd);
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    printf("received from client: %s\n", buff);
    
    /* Изменяем сообщение */
    strcat(buff, "serv");

    /* Отправляпем сообщение клиенту */
    if(sendto(sfd, buff, strlen(buff), 0, (struct sockaddr *) &addr, addr_size) == -1){
        close(sfd);
        perror("sendto error");
        exit(EXIT_FAILURE);
    }
    printf("sent to client: %s\n", buff);

    /* Закрываем дескриптор, соединение моментально обрывается */
    close(sfd);
}