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

#define BUFF_SIZE 50

void main(){
    long int my_time; //переменная для сохранения текущего времени
    int sfd; //файловый дескриптор
    int port_num = 7777; //номер порта
    struct sockaddr_in addr; //структура, хранящая npoint сервера
    socklen_t addr_size; //размер структуры
    char *buff = malloc(BUFF_SIZE);

    //задаем адрес прослушивания и порт
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_num);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    //инициализируем дескриптор сокета
    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(struct sockaddr_in);

    //считываем текущее время
    my_time = time(NULL);
    //с помощью функции ctime преобразуем считанное время в локальное, а затем в строку
    strncpy(buff, ctime(&my_time), BUFF_SIZE);

    //отправка сообщения серверу
    if(sendto(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr, addr_size) == -1){
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }
    printf("sent to server: %s\n", buff);

    free(buff);
    buff = malloc(BUFF_SIZE);

   //принимаем сообщение от сервера
    if(recvfrom(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr, &addr_size) == -1){
        close(sfd);
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    printf("received from server: %s\n", buff);

    free(buff);
    buff = malloc(BUFF_SIZE);
    
    //считываем текущее время
    my_time = time (NULL);
    //с помощью функции ctime преобразуем считанное время в локальное, а затем в строку
    strncpy(buff, ctime(&my_time), BUFF_SIZE);
    //отправляем сообщение серверу
    if(sendto(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr, addr_size) == -1){
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }
    printf("sent to server: %s\n", buff);

    free(buff);
    //закрываем дескриптор, связь моментально обрывается
    close(sfd);
}