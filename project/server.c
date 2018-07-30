#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "server.h"

/* Инициализация сокета и привязка адреса udp */
int udp_bind (struct sockaddr_in *addr_udp, socklen_t addr_size) {
    int u_fd;

    /* Инициализируем дескриптор сокета */
    if ((u_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Задаем адрес и порт */
    memset (addr_udp, 0, addr_size);
    addr_udp->sin_family = AF_INET;
    addr_udp->sin_port = htons (PORT_NUM);
    addr_udp->sin_addr.s_addr = inet_addr ("127.0.0.1");

    /* Связываем адрес с дескриптором сокета */
    if (bind (u_fd, (struct sockaddr *) addr_udp, addr_size) == -1) {
        perror ("bind error in udp");
        exit (EXIT_FAILURE);
    }

    return u_fd;
}

/* Инициализация сокета и привязка адреса tcp */
int tcp_bind (struct sockaddr_in *addr_tcp, socklen_t addr_size) {
    int t_fd;

    /* Инициализируем дескриптор сокета */
    if ((t_fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("error in creating socket in tcp server");
        exit(EXIT_FAILURE);
    }

    /* Задаем адрес прослушивания и порт */
    memset (addr_tcp, 0, addr_size);
    addr_tcp->sin_family = AF_INET;
    addr_tcp->sin_port = htons (PORT_NUM);
    addr_tcp->sin_addr.s_addr = inet_addr ("127.0.0.2");

    /* Связываем адрес с дескриптором сокета */
    if (bind (t_fd, ( struct sockaddr * ) addr_tcp, addr_size) == -1) {
        perror("bind error in tcp");
        exit(EXIT_FAILURE);
    }

    /* Начинаем слушать сокет */
    if (listen (t_fd, LISTEN_NUM) == -1 ) {
        close (t_fd);
        perror ("listen error");
        exit (EXIT_FAILURE);
    }

    return t_fd;
}

/* Настройка широковещательного сокета */
int bc_udp_init(struct sockaddr_in *addr_bc_udp, socklen_t addr_size){
    int bc_fd;
    int val = 1;

    /* Инициализируем дескриптор сокета */
    if ((bc_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Задаем адрес и порт */
    memset (addr_bc_udp, 0, addr_size);
    addr_bc_udp->sin_family = AF_INET;
    addr_bc_udp->sin_port = htons (PORT_NUM);
    addr_bc_udp->sin_addr.s_addr = inet_addr ("192.168.1.255");

    /* Настройка сокета на широковещательную рассылку */
    setsockopt(bc_fd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));

    return bc_fd;
}

/* Контроль ошибок при создании потока */
void errorCreateThread (int result) {
    if (result != 0) {
        perror ("Error in creating the thread");
        exit (EXIT_FAILURE);
    }
}

/* Контроль ошибок при ожидании завершения потока */
void errorJoinThread (void *status) {
    if (status != 0) {
        perror ("Error in joining the thread");
        exit (EXIT_FAILURE);
    }
}

/* Функция потока - выполняет широковещательную рассылку адреса и порта главного udp сервера */
void *broadcast (void *arg) {
    struct thread_data *my_data = (struct thread_data *) arg;
    struct sockaddr_in addr_client;
    socklen_t addr_size = sizeof (struct sockaddr_in);

    while(1) {
        if(sendto (my_data->broadcast_fd, &(my_data->udp_address), addr_size, 0, (struct sockaddr *) &(my_data->udp_bc_address), addr_size) == -1) {
            close (my_data->broadcast_fd);
            perror ("sendto error");
            exit (EXIT_FAILURE);
        }
        printf ("data was sent to client\n");
        sleep (1);
    }

    pthread_exit (NULL);
}

/* Карта заполняется единичками */
void create_map (int map[ROW][COL]) {
    int i, j;

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            map[i][j] = 1;
        }
    }
}

/* Назначение игроку начальных координат */
void init_start_xy (int map_state[ROW][COL], int *start_xy) {
    int i, j;

    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if (map_state[i][j] == 0) {
                start_xy[0] = i; //y-координата
                start_xy[1] = j; //x-координата
                map_state[i][j] = 1;
                return;
            }
        }
    }
}

/* Заносим клиента в массив */
void add_client (struct sockaddr_in *clients, struct sockaddr_in addr_client) {
    int i;

    for (i = 0; i < CLIENTS_NUM; i++) {
        if ((clients[i].sin_port == 0) && (clients[i].sin_family == 0) && (clients[i].sin_addr.s_addr == 0)) {
            clients[i] = addr_client;
            return;
        }
    }
}

/* Ищем клиента в массиве */
int search_client (struct sockaddr_in *clients, struct sockaddr_in addr_client) {
    int i;
    int num;

    for (i = 0; i < CLIENTS_NUM; i++) {
        if ((clients[i].sin_addr.s_addr == addr_client.sin_addr.s_addr) && (clients[i].sin_port == addr_client.sin_port) && (clients[i].sin_family == addr_client.sin_family)) {
            num = i;
            break;
        }
    }

    return num;
}

/* Удаляем клиента из массива */
void delete_client (struct sockaddr_in *clients, struct sockaddr_in addr_client) {
    int num;
    socklen_t addr_size = sizeof (struct sockaddr_in);

    num = search_client (clients, addr_client);
    memset (clients + num*addr_size, 0, addr_size);
}

void main (){
	pthread_attr_t attr; //атрибут для создания потока
    pthread_t thread;
    struct sockaddr_in addr_tcp, addr_udp, addr_bc_udp, addr_client;
    struct epoll_event events[2]; //события, которые мы ждём
    struct epoll_event event_happened; //событие, которое произошло
    socklen_t addr_size = sizeof (struct sockaddr_in);
    struct thread_data udp_data;
    struct sockaddr_in *clients = malloc(sizeof(struct sockaddr_in) * CLIENTS_NUM);
    void *status;
    char *msg;
    int epfd; //экземпляр опроса событий
    int u_fd, t_fd, bc_fd, s_fd; //дескрипторы udp и tcp сокетов
    int result;
    int i,j;
    int start_xy[2]; //начальные координаты игрока
    int map[ROW][COL];
    int map_state[ROW][COL]; //состояние карты - координаты игроков


    create_map (map);

    memset (clients, 0, addr_size * CLIENTS_NUM); //обнуляем массив структур
    memset (&map_state, 0, ROW*COL*sizeof(int)); //все координаты для игроков изначально свободны
    
    u_fd = udp_bind (&addr_udp, addr_size);
    t_fd = tcp_bind (&addr_tcp, addr_size);
    bc_fd = bc_udp_init (&addr_bc_udp, addr_size);

    /* Заполняем структуру-параметр функции потока */
    udp_data.broadcast_fd = bc_fd;
    udp_data.udp_address = addr_udp;
    udp_data.udp_bc_address = addr_bc_udp;

    /* Инициализация и установка атрибута */
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

    result = pthread_create (&thread, &attr, broadcast, (void*) &udp_data); //создание потока
    errorCreateThread (result); //контроль ошибок

    /* Создаём контекст опроса событий  */
    epfd = epoll_create1 (0);
    if (epfd < 0) {
        perror ("epoll_create1");
        exit (EXIT_FAILURE);
    }

    /* Добавляем к экземпляру опроса событий epfd новое наблюдение 
    для файла, связанного с дескриптором t_fd*/
    events[0].data.fd = t_fd;
    events[0].events = EPOLLIN | EPOLLET;
    result = epoll_ctl (epfd, EPOLL_CTL_ADD, t_fd, &events[0]);
    if (result) {
        perror ("epoll_ctl tcp");
        exit (EXIT_FAILURE);
    }

    /* Добавляем к экземпляру опроса событий epfd новое наблюдение 
    для файла, связанного с дескриптором u_fd*/
    events[1].data.fd = u_fd;
    events[1].events = EPOLLIN | EPOLLET;
    result = epoll_ctl (epfd, EPOLL_CTL_ADD, u_fd, &events[1]);
    if (result) {
        perror ("epoll_ctl udp");
        exit(EXIT_FAILURE);
    }

    while(1){
    	/* Все установлено, блокируем */
        result = epoll_wait (epfd, &event_happened, 1, -1);
        if (result < 0) {
            perror ("epoll_wait");
        }

        /* Подключающемуся клиенту отправляется карта и связь по tcp обрывается */
        if (event_happened.data.fd == t_fd) { //проверка на событие
            if ((s_fd = accept (t_fd, (struct sockaddr *) &addr_client, &addr_size)) == -1) {
                perror ("accept error");
            }

            if (send (s_fd, &map, ROW*COL*sizeof(int), 0) == -1) {
                close(s_fd);
                perror("send error");
            }
            printf("map was sent to client\n");

            close(s_fd);
        }

        /* Сервер ждёт сообщение от клиента по udp, заносит его данные в массив
        и отправляет клиенту координаты начального положения*/
        if (event_happened.data.fd == u_fd) { //проверка на событие
            msg = malloc(MSG_SIZE);

            /* Принимаем сообщение от клиента, сохраняем его данные в массив*/
            if (recvfrom(u_fd, msg, MSG_SIZE, 0, (struct sockaddr *) &addr_client, &addr_size) == -1) {
                perror("recvfrom error");
                free(msg);
            }
            printf("received from client: %s\n", msg);
            free(msg);

            /* Заносим клиента в массив */
            add_client(clients, addr_client);

            /* Выбираем для клиента свободную координату на карте */
            init_start_xy(map_state, start_xy);

            /* Отправляем клиенту его координату на карте */
            if (sendto(u_fd, &start_xy, sizeof(int)*2, 0, (struct sockaddr *) &addr_client, addr_size) == -1) {
                perror("sendto error");
            }
            printf("coordinates were sent to client");
            delete_client(clients, addr_client);
        }
    }


    pthread_join (thread, &status); 
    errorJoinThread (status); //контроль ошибок
    
    pthread_attr_destroy (&attr);
    free(clients);
    close (t_fd);
    close (u_fd);
    exit (EXIT_SUCCESS);
}
