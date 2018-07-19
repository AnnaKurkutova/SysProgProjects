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

/* Приветственное сообщение серверу */ 
void welcome_msg(int sfd){
    socklen_t addr_size; //размер структуры
    char buff = 'H';

    addr_size = sizeof(struct sockaddr_in);

    if(send(sfd, &buff, 1, 0) == -1){
        close(sfd);
        perror("sendto error");
        exit(EXIT_FAILURE);
    }
    printf("sent to server: %c\n", buff);

}

/* Функция отправки текущего времени серверу */
void send_time_to_server(int sfd){
    long int my_time; //переменная для сохранения текущего времени
    char buff[BUFF_SIZE];

    my_time = time (NULL); //считываем текущее время
    strncpy(buff, ctime(&my_time), BUFF_SIZE); //считанное время  переводим в локальное, а затем в строку

    if(send(sfd, buff, BUFF_SIZE, 0) == -1){ //отправляем сообщение серверу
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }
    printf("sent to server: %s\n", buff);
}

/* Функция получения сообщения от сервера */
void recv_msg_from_server(int sfd){
    char buff[BUFF_SIZE];

    if(recv(sfd, buff, BUFF_SIZE, 0) == -1){//принимаем сообщение от сервера
        close(sfd);
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    printf("received from server: %s\n", buff);
}

void main(){
    int sfd; //дескриптор сокета
    int port_num = 7777; //номер порта
    struct sockaddr_in my_addr; //структура, хранящая npoint клиента/сервера

    memset(&my_addr, 0, sizeof(struct sockaddr_in)); //зануляем структуру
    my_addr.sin_family = AF_INET; //задаём семейство
    my_addr.sin_port = htons(port_num); //задаём порт
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //задаём адрес прослушивания

    if((sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){ //инициализируем дескриптор сокета
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    if(connect(sfd, (struct sockaddr*) &my_addr, sizeof(my_addr)) == -1){ //соединяемся с сервером
        close(sfd);
        perror("connection error");
        exit(EXIT_FAILURE);
    }
    
    welcome_msg(sfd);
    recv_msg_from_server(sfd);
    send_time_to_server(sfd);

    close(sfd); //закрываем дескриптор, связь моментально обрывается
}
