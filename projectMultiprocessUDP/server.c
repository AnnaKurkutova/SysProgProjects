#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <time.h> 

#define BUFF_SIZE 50

void main(){
    long int my_time; //переменная для сохранения текущего времени
    int sfd, cfd; //файловые дескрипторы
    int i = 0;
    int status; //статус завершения дочернего процесса
    int port_num = 7777;
    struct sockaddr_in addr_first, addr_second; //структуры для хранения npoint клиента и сервера
    socklen_t addr_size; //размер структуры
    pid_t pid;
    char *buff = malloc(BUFF_SIZE); //строка с сообщением

    //задаем адрес и порт
    memset(&addr_first, 0, sizeof(struct sockaddr_in));
    addr_first.sin_family = AF_INET;
    addr_first.sin_port = htons(port_num);
    addr_first.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    //инициализируем дескриптор сокета
    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    //связываем адрес с дескриптором сокета
    if((bind(sfd, (struct sockaddr *) &addr_first, sizeof(struct sockaddr_in))) == -1){
        close(sfd);
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(struct sockaddr_in);

    //принимаем сообщение от клиента, сохраняем его npoint
    if(recvfrom(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr_first, &addr_size) == -1){
        close(sfd);
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    printf("received from client: %s\n", buff);

    //создание процесса-потомка
    switch(pid = fork()){
        case -1: //ошибка
            close(sfd);
            perror("fork error");
            exit(EXIT_FAILURE);

        case 0: //дочерний процесс
            close(sfd);
            //задаем адрес и порт
            memset(&addr_second, 0, sizeof(struct sockaddr_in));
            port_num++;
            addr_second.sin_family = AF_INET;
            addr_second.sin_port = htons(port_num);
            addr_second.sin_addr.s_addr = inet_addr("127.0.0.1"); 
            //инициализируем дескриптор сокета
            if((cfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
                close(cfd);
                perror("error in creating socket");
                exit(EXIT_FAILURE);
            }
            //отправляем клиенту сообщение, клиент получает новый npoint
            free(buff);
            buff = malloc(BUFF_SIZE);
            //считываем текущее время
            my_time = time(NULL);
            //с помощью функции ctime преобразуем считанное время в локальное, а затем в строку
            strncpy(buff, ctime(&my_time), BUFF_SIZE);
            //отправляем сообщение клиенту
            if(sendto(cfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr_first, addr_size) == -1){
                close(cfd);
                perror("sendto error");
                exit(EXIT_FAILURE);
            }
            printf("sent to client: %s\n", buff);
            free(buff);
            buff = malloc(BUFF_SIZE);
            //принимаем сообщение от клиента
            if(recvfrom(cfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &addr_second, &addr_size) == -1){
                close(cfd);
                perror("recvfrom error");
                exit(EXIT_FAILURE);
            }
            printf("received from client: %s\n", buff);
            //закрываем дескриптор
            close(cfd);
            i++;
            //завершаем процесс
            exit(EXIT_SUCCESS);
            break;

        default:
            if(wait(&status) == -1){
                close(sfd);
                perror("wait error");
                exit(EXIT_FAILURE);
            }
    }


    //закрываем дескриптор, соединение моментально обрывается 
    close(sfd);
}