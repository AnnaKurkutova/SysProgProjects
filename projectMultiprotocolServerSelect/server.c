#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stddef.h> 
#include <time.h>

/********************************************************************
*  Данная программа является мультипротокольным сервером TCP/UDP.   *
*  В программе используется интерфейс для выбора(select). В набор   *
*  блокирования на чтение заносятся два дескриптора сокетов t_fd    *
*  и u_fd. Когда один из клиентов TCP/UDP подключается к серверу,   *
*  происходит снятие блокировки и выполняются приём сообщения от    *
*  клиента и отправка сообщения с текущим временем клиенту.         *
********************************************************************/

#define LISTEN_NUM 5
#define TIMEOUT 10       // установка тайм-аута в секундах
#define BUFF_SIZE 100    // длина буфера считывания в байтах

void main(){
	long int my_time; //переменная для сохранения текущего времени
	int ret;
    int u_fd, t_fd, max_fd; //дескрипторы
    int port_tcp = 7777, port_udp = 7778; //порты
    struct timeval tv; //время
    struct sockaddr_in addr_tcp, addr_udp; //структуры с npoint
    socklen_t addr_size; //размер структуры
    fd_set readfds; //набор блокировки на чтение
    char *buff; //строка с сообщением

            /*==========================*
             *  Работа с TCP-сервером   *
             *==========================*/

    /* Задаем адрес прослушивания и порт для tcp сервера */
    memset(&addr_tcp, 0, sizeof(struct sockaddr_in));
    addr_tcp.sin_family = AF_INET;
    addr_tcp.sin_port = htons(port_tcp);
    addr_tcp.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Инициализируем дескриптор сокета для tcp сервера */
    if((t_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        perror("error in creating socket in tcp server");
        exit(EXIT_FAILURE);
    }

    /* Связываем адрес с дескриптором слушающего сокета для tcp сервера */
    if((bind(t_fd, (struct sockaddr *) &addr_tcp, sizeof(struct sockaddr_in))) == -1){
        perror("bind error in tcp server");
        exit(EXIT_FAILURE);
    }

    /* Начинаем слушать сокет */
    if(listen(t_fd, LISTEN_NUM) == -1){
        close(t_fd);
        perror("listen error");
        exit(EXIT_FAILURE);
    }
            /*==========================*
             *  Работа с UDP-сервером   *
             *==========================*/

    /* Задаем адрес прослушивания и порт для udp сервера */
    memset(&addr_udp, 0, sizeof(struct sockaddr_in));
    addr_udp.sin_family = AF_INET;
    addr_udp.sin_port = htons(port_udp);
    addr_udp.sin_addr.s_addr = inet_addr("127.0.0.2");

    /* Инициализируем дескриптор сокета для udp сервера */
    if((u_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket in udp server");
        exit(EXIT_FAILURE);
    }

    /* Связываем адрес с дескриптором сокета для udp сервера */
    if((bind(u_fd, (struct sockaddr *) &addr_udp, sizeof(struct sockaddr_in))) == -1){
        close(u_fd);
        perror("bind error in udp server");
        exit(EXIT_FAILURE);
    }


    addr_size = sizeof(struct sockaddr_in);

    while(1){
        FD_ZERO(&readfds); //обнуление
        FD_SET(u_fd, &readfds); //добавляем дескриптор u_fd в набор на чтение
        FD_SET(t_fd, &readfds); //добавляем дескриптор t_fd в набор на чтение

        /* Устанавливаем максимальный дескриптор */
        if(u_fd > t_fd){
            max_fd = u_fd + 1;
        }
        else{
            max_fd = t_fd + 1;
        }
        
        tv.tv_sec = TIMEOUT; //ожидаем не больше 10 секунд
        tv.tv_usec = 0;
        
        ret = select (max_fd, &readfds, NULL, NULL, &tv); //блокировка на чтение
        if (ret == -1) { //не удалось блокировать
            perror ("select");
            exit(EXIT_FAILURE);
        } 
        else if (!ret) { //время истекло
            printf ("%d seconds elapsed\n", TIMEOUT);
        }  

        if(FD_ISSET(t_fd, &readfds)){ //проверка на наличие дескриптора в наборе
        	int cfd;
        	buff = malloc(BUFF_SIZE);

            /* Принимаем соединение, создаём дескриптор для взаимодействия с конкретным клиентом */
            if((cfd = accept(t_fd, (struct sockaddr *) &addr_tcp, &addr_size)) == -1){
                close(t_fd);
                perror("accept error");
                exit(EXIT_FAILURE);
            }

            /* Принимаем сообщение от клиента */
            if(recv(cfd, buff, BUFF_SIZE, 0) == -1){
                close(cfd);
                perror("recv error");
                exit(EXIT_FAILURE);
            }
            printf("received from tcp client: %s\n", buff);
            free(buff);

            buff = malloc(BUFF_SIZE);
            my_time = time(NULL); //считываем текущее время
            strncpy(buff, ctime(&my_time), BUFF_SIZE); //с помощью функции ctime преобразуем считанное время в локальное, а затем в строку
            
            /* Отправляем сообщение клиенту */
            if(send(cfd, buff, BUFF_SIZE, 0) == -1){
                close(cfd);
                perror("send error");
                exit(EXIT_FAILURE);
            }
            printf("sent to tcp client: %s\n", buff);
            free(buff);

            close(cfd);
        }

        if(FD_ISSET(u_fd, &readfds)) { //проверка на наличие дескриптора в наборе
        	buff = malloc(BUFF_SIZE);

            /* Принимаем сообщение от клиента, сохраняем его npoint */
            if(recvfrom(u_fd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr_udp, &addr_size) == -1){
                close(u_fd);
                perror("recvfrom error");
                exit(EXIT_FAILURE);
            }
            printf("received from udp client: %s\n", buff);
            free(buff);

            buff = malloc(BUFF_SIZE);
            my_time = time(NULL); //считываем текущее время
            strncpy(buff, ctime(&my_time), BUFF_SIZE); //с помощью функции ctime преобразуем считанное время в локальное, а затем в строку
            
            /* Отправляем сообщение клиенту */
            if(sendto(u_fd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr_udp, addr_size) == -1){
                close(u_fd);
                perror("sendto error");
                exit(EXIT_FAILURE);
            }
            printf("sent to udp client: %s\n", buff);
            free(buff);
        }
    }

    close(t_fd);
    close(u_fd);
    exit(EXIT_SUCCESS);
}


