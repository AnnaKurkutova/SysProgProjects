#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 100

void main(){
    int sfd; //дескриптор сокета
    struct sockaddr_in send_addr; //структура, хранящая npoint
    socklen_t addr_size; //размер структуры
    char buff[BUFF_SIZE]; //строка с сообщением

    /* Инициализируем дескриптор сокета */
    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Задаем адрес и порт */
    memset(&send_addr, 0, sizeof(struct sockaddr_in));
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(7777);
    send_addr.sin_addr.s_addr = inet_addr("192.168.1.255");

    /* Связываем адрес с дескриптором сокета */
    if((bind(sfd, (struct sockaddr *) &send_addr, sizeof(struct sockaddr_in))) == -1){
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    /* Принимаем сообщение */
    if(recvfrom(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &send_addr, &addr_size) == -1){
        close(sfd);
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    printf("received: %s\n", buff);

    close(sfd);
    exit(EXIT_SUCCESS);
}