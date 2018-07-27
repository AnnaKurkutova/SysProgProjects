#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/************************************************************************************
*  Данная программа является UDP-клиентом на raw сокетах. В отправляемом серверу    *
*  сообщении UDP-заголовок заполняется вручную. Для этого используется структура    *
*  my_udp_hdr. Структура содержит порт отправителя, порт получателя, общую длину    *
*  заголовка и контрольную сумму. Затем клиент получает ответное сообщение от       *
*  сервера и сравнивает destination port в UDP-заголовке с source port клиента.     *
*  При удачном сравнении считатся, что сообщение от сервера доставлено успешно.     *
************************************************************************************/

#define MSG_SIZE 100
#define UDP_HDR_SIZE 8
#define IP_HDR_SIZE 20
#define ONE_FIELD_SIZE 2
#define DEST_PORT 7777
#define SOURCE_PORT 9999

struct my_udp_hdr{
    u_int16_t sourse_addr;
    u_int16_t dest_addr;
    u_int16_t lenth;
    u_int16_t check_sum;

}__attribute__((packed));

/* Функция ввода сообщения с клавиатуры */
void write_msg(char *buff){
    int ch;
    int i = 0;
    
    while (((ch = getchar()) != '\n') && (i < MSG_SIZE)) { //fgets
        buff[i++] = ch;
    }
    buff[i] = '\0';
}

void main(){
    int sfd; //файловый дескриптор
    int total_size;
    int recv_size;
    short int *ptr;
    struct sockaddr_in dest_addr;
    struct my_udp_hdr *udph;
    socklen_t addr_size;
    char *buff = malloc(MSG_SIZE + UDP_HDR_SIZE);
    char *msg;

    /* Задаем адрес назначения (сервера) и порт */
    memset(&dest_addr, 0, sizeof(struct sockaddr_in));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    /* Инициализируем дескриптор raw сокета */
    if((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Вводим сообщение для отправки */
    memset(buff, 0, MSG_SIZE + UDP_HDR_SIZE); //зануляем буфер
    printf("enter the message: ");
    msg = buff + UDP_HDR_SIZE;
    write_msg(msg);
    total_size = UDP_HDR_SIZE + strlen(msg);

    /* Заполняем UDP заголовок */
    udph = (struct my_udp_hdr *)buff;
    udph->sourse_addr = htons(SOURCE_PORT); 
    udph->dest_addr = htons(DEST_PORT);
    udph->lenth = htons(total_size); 
    //udph->check_sum = 0; 

    addr_size = sizeof(struct sockaddr_in);
    
    /* Отправляем сообщение серверу */
    if(sendto(sfd, buff, total_size, 0, (struct sockaddr *) &dest_addr, addr_size) == -1){
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }

    printf("sent to server: %s\n", buff + UDP_HDR_SIZE);

    recv_size = MSG_SIZE + IP_HDR_SIZE + UDP_HDR_SIZE;

    while(1){
        free(buff);
        buff = malloc(recv_size);

        /* Принимаем сообщение от сервера */
        if(recvfrom(sfd, buff, recv_size, 0, (struct sockaddr *) &dest_addr, &addr_size) == -1){
            close(sfd);
            perror("recv error");
            exit(EXIT_FAILURE);
        }
        
        /* Проверяем, совпадает ли destination port в полученном пакете с source port клиента  */
        ptr = (int short *)(buff + IP_HDR_SIZE + ONE_FIELD_SIZE);
        ntohs(*ptr);

        if(*ptr == htons(SOURCE_PORT)){
            printf("received from server: %s\n", buff + UDP_HDR_SIZE + IP_HDR_SIZE);
            free(buff);
            break;
        }
    }

    /* Закрываем дескриптор, связь моментально обрывается */
    close(sfd);
}