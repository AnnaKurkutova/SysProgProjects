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
#define BUFF_SIZE 50

void main(){
    int sfd, cfd; //файловые дескрипторы
    struct sockaddr_in my_addr, peer_adr;
    socklen_t peer_addr_size;
    char buff[BUFF_SIZE];

    //задаем адрес прослушивания и порт
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = IPPROTO_TCP;
    my_addr.sin_addr.s_addr = INADDR_ANY; 

    //инициализируем дескриптор сокета
    if((sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    //связываем адрес с дескриптором слушающего сокета
    if((bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in))) == -1){
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    //начинаем слушать сокет
    if(listen(sfd, LISTEN_NUM) == -1){
        close(sfd);
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    peer_addr_size = sizeof(struct sockaddr_in);

    //принимаем соединение, создаём дескриптор для взаимодействия с конкретным клиентом
    if((cfd = accept(sfd, (struct sockaddr *) &peer_adr, &peer_addr_size)) == -1){
        close(sfd);
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    
    //принимаем сообщение от клиента
    if(recv(cfd, buff, BUFF_SIZE, 0) == -1){
        close(cfd);
        close(sfd);
        perror("recv error");
        exit(EXIT_FAILURE);
    }
    printf("received from client: %s\n", buff);

    //изменяем сообщение
    strncat(buff, "serv", BUFF_SIZE);

    //отправляпем сообщение клиенту
    if(send(cfd, buff, BUFF_SIZE, 0) == -1){
        close(cfd);
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }
    printf("sent to client: %s\n", buff);

    //закрываем дескрипторы, соединение моментально обрывается 
    close(cfd);
    close(sfd);
}