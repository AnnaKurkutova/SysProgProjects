#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stddef.h> 
#include <time.h>

/******************************************************************************
*  Данная программа является мультипротокольным сервером TCP/UDP.             *
*  В программе используется интерфейс для расширенного опроса epoll().        *
*  Контекст опроса событий создается с помощью вызова epoll_create1().        *
*  Системный вызов epoll_ctl() можно использовать для добавления файловых     *
*  дескрипторов в заданный контекст опроса и удаления этих дескрипторов из    *
*  контекста. Системный вызов epoll_wait() ожидает событий на файловых        *
*  дескрипторах, ассоциированных с заданным экземпляром опроса событий.       *
*  Когда один из клиентов TCP/UDP подключается к серверу, происходит снятие   *
*  блокировки и выполняются приём сообщения от клиента и отправка сообщения   *
*  с текущим временем клиенту.                                                *
******************************************************************************/

#define LISTEN_NUM 5
#define TIMEOUT 10       // установка тайм-аута в секундах
#define BUFF_SIZE 100    // длина буфера считывания в байтах

void main(){
    long int my_time; //переменная для сохранения текущего времени
    int ret;
    int epfd; //экземпляр опроса событий
    int nr_events;
    int u_fd, t_fd; //дескрипторы
    int port_tcp = 7777, port_udp = 7778; //порты
    struct sockaddr_in addr_tcp, addr_udp; //структуры с npoint
    socklen_t addr_size; //размер структуры
    struct epoll_event events[2]; //события
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

    /* Создаём контекст опроса событий  */
    epfd = epoll_create1 (0);
    if (epfd < 0) {
        perror ("epoll_create1");
        exit(EXIT_FAILURE);
    }

    /* Добавляем к экземпляру опроса событий epfd новое наблюдение 
    для файла, связанного с дескриптором t_fd*/
    events[0].data.fd = t_fd; // вернуть нам t_fd позже (от epoll_wait) 
    events[0].events = EPOLLIN | EPOLLET; 
    ret = epoll_ctl (epfd, EPOLL_CTL_ADD, t_fd, &events[0]);
    if (ret) {
        perror ("epoll_ctl tcp");
        exit(EXIT_FAILURE);
    }

    /* Добавляем к экземпляру опроса событий epfd новое наблюдение 
    для файла, связанного с дескриптором u_fd*/
    events[1].data.fd = u_fd; // вернуть нам u_fd позже (от epoll_wait) 
    events[1].events = EPOLLIN | EPOLLET; 
    ret = epoll_ctl (epfd, EPOLL_CTL_ADD, u_fd, &events[1]);
    if (ret) {
        perror ("epoll_ctl udp");
        exit(EXIT_FAILURE);
    }

    while(1){
        /* Все установлено, блокируем */
        nr_events = epoll_wait (epfd, events, 1, -1);
        if (nr_events < 0) {
            perror ("epoll_wait");
            exit(EXIT_FAILURE);
        }

        if(events[0].data.fd == t_fd){ //проверка на событие
            int cfd;
            buff = malloc(BUFF_SIZE);

            /* Принимаем соединение, создаём дескриптор для взаимодействия с конкретным клиентом */
            if((cfd = accept(t_fd, (struct sockaddr *) &addr_tcp, &addr_size)) == -1){
                close(t_fd);
                perror("accept error");
                free(buff);
                exit(EXIT_FAILURE);
            }

            /* Принимаем сообщение от клиента */
            if(recv(cfd, buff, BUFF_SIZE, 0) == -1){
                close(cfd);
                perror("recv error");
                free(buff);
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
                free(buff);
                exit(EXIT_FAILURE);
            }
            printf("sent to tcp client: %s\n", buff);
            free(buff);

            close(cfd);
        }

        if(events[1].data.fd == u_fd) { //проверка на событие
            buff = malloc(BUFF_SIZE);

            /* Принимаем сообщение от клиента, сохраняем его npoint */
            if(recvfrom(u_fd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr_udp, &addr_size) == -1){
                close(u_fd);
                perror("recvfrom error");
                free(buff);
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
                free(buff);
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


