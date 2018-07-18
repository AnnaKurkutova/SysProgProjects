#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define BUFF_SIZE 100

void main(){
    long int my_time; //переменная для сохранения текущего времени
    int sfd; //дескриптор сокета
    int port_num = 7777;
    struct sockaddr_in my_addr; //структура, хранящая npoint клиента/сервера
    char *buff = malloc(BUFF_SIZE);

    //задаем адрес прослушивания и порт
    memset(&my_addr, 0, sizeof(struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port_num);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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

    //считываем текущее время
    my_time = time (NULL);
    //с помощью функции ctime преобразуем считанное время в локальное, а затем в строку
    strncpy(buff, ctime(&my_time), BUFF_SIZE);
    
    //отправляем сообщение серверу
    if(send(sfd, buff, BUFF_SIZE, 0) == -1){
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }
    printf("sent to server: %s\n", buff);

    free(buff);
    //закрываем дескриптор, связь моментально обрывается
    close(sfd);
}