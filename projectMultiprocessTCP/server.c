#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stddef.h> 

#define LISTEN_NUM 5
#define BUFF_SIZE 100

void main(){
    int sfd, cfd; //дескрипторы сокетов
    int ch;
    int port_num = 7777;
    struct sockaddr_in my_addr, peer_adr; //структуры, хранящие npoint клиента/сервера
    socklen_t peer_addr_size; //размер структуры
    pid_t pid;
    char buff[BUFF_SIZE]; //строка с сообщением

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

    //связываем адрес с дескриптором слушающего сокета
    if((bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in))) == -1){
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    //начинаем слушать сокет
    if(listen(sfd, LISTEN_NUM) == -1){
        close(sfd);
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    peer_addr_size = sizeof(struct sockaddr_in);

    while(1){
        //принимаем соединение, создаём дескриптор для взаимодействия с конкретным клиентом
        if((cfd = accept(sfd, (struct sockaddr *) &peer_adr, &peer_addr_size)) == -1){
            close(sfd);
            perror("accept error");
            exit(EXIT_FAILURE);
        }
        switch(pid = fork()){
            case 0: //дочерний процесс
                close(sfd);
                //принимаем сообщение от клиента
                if(recv(cfd, buff, BUFF_SIZE, 0) == -1){
                    close(cfd);
                    perror("recv error");
                    exit(EXIT_FAILURE);
                }
                printf("received from client: %s\n", buff);
                
                close(cfd);
                exit(EXIT_FAILURE);
                break;

            case -1: //ошибка
                close(cfd);
                close(sfd);
                perror("fork");
                exit(EXIT_FAILURE);
            
            default: //родительский процесс
                close(cfd);
        }
    }

    //закрываем дескриптор, соединение моментально обрывается 
    close(sfd);
}