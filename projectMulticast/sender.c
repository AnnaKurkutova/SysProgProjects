#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define BUFF_SIZE 100

/********************************************************************************
*  В данной программе для доставки пакетов различным получателям используется   *
*  многоадресная IP-рассылка. Адреса IPv4 Class D (то есть со значениями в      *
*  диапазоне от 224.0.0.0 до 239.255.255.255) зарезервированы для               *
*  многоадресных рассылок. Каждый адрес может быть связан с группой             *
*  многоадресной рассылки. Чтобы получить данные, переданные по этому адресу,   *
*  необходимо стать членом такой группы. Был выбран адрес 224.0.0.1.            *
********************************************************************************/

void main(){
    long int my_time; //переменная для сохранения текущего времени
    int sfd; //дескриптор сокета
    struct sockaddr_in recv_addr; //структура, хранящая npoint
    socklen_t addr_size; //размер структуры
    char buff[BUFF_SIZE]; //строка с сообщением

    /* Инициализируем дескриптор сокета */
    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Задаем адрес и порт */
    memset(&recv_addr, 0, sizeof(struct sockaddr_in));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(7777);
    recv_addr.sin_addr.s_addr = inet_addr("224.0.0.1");

    my_time = time(NULL); //считываем текущее время
    strncpy(buff, ctime(&my_time), BUFF_SIZE); //с помощью функции ctime преобразуем считанное время в локальное, а затем в строку
    
    addr_size = sizeof(struct sockaddr_in); //размер структуры

    /* Отправляем сообщение */
    if(sendto(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &recv_addr, addr_size) == -1){
        close(sfd);
        perror("sendto error");
        exit(EXIT_FAILURE);
    }
    printf("sent to client: %s\n", buff);

    close(sfd);
    exit(EXIT_SUCCESS);
}