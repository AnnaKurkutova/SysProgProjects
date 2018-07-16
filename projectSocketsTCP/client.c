#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

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
    char *buff = malloc(BUFF_SIZE);

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

    //соединяемся с сервером
    if(connect(sfd, (struct sockaddr*) &my_addr, sizeof(my_addr)) == -1){
        close(sfd);
        perror("connection error");
        exit(EXIT_FAILURE);
    }

    //вводим сообщение для отправки
    printf("enter the message: ");
    write_msg(buff);
    
    //отправляем сообщение серверу
    if(send(sfd, buff, BUFF_SIZE, 0) == -1){
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }
    printf("sent to server: %s\n", buff);

    free(buff);
    buff = malloc(BUFF_SIZE);

    //принимаем сообщение от сервера
    if(recv(sfd, buff, BUFF_SIZE, 0) == -1){
        close(sfd);
        perror("recv error");
        exit(EXIT_FAILURE);
    }
    printf("received from server: %s\n", buff);

    free(buff);
    //закрываем дескриптор, связь моментально обрывается
    close(sfd);
}