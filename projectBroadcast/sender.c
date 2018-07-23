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

/**************************************************************************************
*  В данной программе реализована широковещательная рассылка (broadcast),             *
*  то есть рассылка всем хостам, подключенным к данной сети, а не какому-то           *
*  конкретному хосту. Адрес сети 192.168.1.0, маска сети 255.255.255.0,               *
*  широковещательный адрес 192.168.1.255. Для реализации широковещательной            *
*  рассылки на сокетах, необходимо настроить сокет. Настройка производится с          *
*  помощью метода setsockopt(). Функция setsockopt() вызывается со следующими         *
*  параметрами: sfd - дескриптор сокета, SOL_SOCKET - уровень сокета,                 *
*  SO_BROADCAST - возможность отправки широковещательных сообщений, val = 1 -         *
*  активация функции, sizeof(val). В качестве сообщения отправляется текущее время.   *
**************************************************************************************/

void main(){
	long int my_time; //переменная для сохранения текущего времени
    int sfd; //дескриптор сокета
    int val = 1;
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
    recv_addr.sin_addr.s_addr = inet_addr("192.168.1.255");
    
    setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val)); //настройка сокета

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