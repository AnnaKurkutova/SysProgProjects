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
void welcome_msg(int sfd, struct sockaddr_in my_addr){
    socklen_t addr_size; //размер структуры
    char buff = 'H';

    addr_size = sizeof(struct sockaddr_in);

    if(sendto(sfd, &buff, 1, 0, (struct sockaddr *) &my_addr, addr_size) == -1){
        close(sfd);
        perror("sendto error");
        exit(EXIT_FAILURE);
    }
    printf("sent to server: %c\n", buff);

}

/* Функция получения сообщения от сервера */
void recv_msg_from_server(int sfd, struct sockaddr_in my_addr){
    socklen_t addr_size; //размер структуры
    char buff[BUFF_SIZE];

    addr_size = sizeof(struct sockaddr_in);

    if(recvfrom(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &my_addr, &addr_size) == -1){
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
    my_addr.sin_port = htons(port_num); //задаем порт
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //задаем адрес прослушивания

    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){ //инициализируем дескриптор сокета
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    welcome_msg(sfd, my_addr);
    recv_msg_from_server(sfd, my_addr);

    //закрываем дескриптор, связь моментально обрывается
    close(sfd);
}
