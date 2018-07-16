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
    int sfd; //файловый дескриптор
    struct sockaddr_in my_addr;
    socklen_t addr_size;
    char buff[BUFF_SIZE];

    //задаем адрес и порт
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = IPPROTO_UDP;
    my_addr.sin_addr.s_addr = INADDR_ANY; 

    //инициализируем дескриптор сокета
    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    //связываем адрес с дескриптором сокета
    if((bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in))) == -1){
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(struct sockaddr_in);

    //принимаем сообщение от клиента
    if(recvfrom(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &my_addr, &addr_size) == -1){
        close(sfd);
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    printf("received from client: %s\n", buff);

    //изменяем сообщение
    strncat(buff, "serv", BUFF_SIZE);

    //отправляпем сообщение клиенту
    if(sendto(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &my_addr, addr_size) == -1){
        close(sfd);
        perror("sendto error");
        exit(EXIT_FAILURE);
    }
    printf("sent to client: %s\n", buff);

    //закрываем дескриптор, соединение моментально обрывается 
    close(sfd);
}