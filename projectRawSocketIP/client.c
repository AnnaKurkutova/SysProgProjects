#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/*************************************************************************************
*  Данная программа является UDP-клиентом на raw сокетах. В отправляемом серверу     *
*  сообщении IP-заголовок и UDP-заголовок заполняются вручную. Для этого             *
*  используются структуры ip и udphdr соответственно. После заполнения заголовков    *
*  клиент набирает и отправляет сообщение серверу. Затем клиент получает ответное    *
*  сообщение от сервера и сравнивает destination port в UDP-заголовке с source       *
*  port клиента. При удачном сравнении считатся, что сообщение от сервера            *
*  доставлено успешно.                                                               *
*************************************************************************************/

#define MSG_SIZE 100
#define UDP_HDR_SIZE 8
#define IP_HDR_SIZE 20
#define ONE_FIELD_SIZE 2
#define DEST_PORT 7777
#define SOURCE_PORT 9999

/* Функция ввода сообщения с клавиатуры */
void write_msg(char *buff){
    int ch;
    int i = 0;
    
    while (((ch = getchar()) != '\n') && (i < MSG_SIZE)) {
        buff[i++] = ch;
    }
    buff[i] = '\0';
}

void main(){
    int sfd; //файловый дескриптор
    int val = 1;
    int total_size; //общий размер отправляемого сообщения (с заголовками)
    int recv_size; //размер получаемого сообщения
    short int *ptr; 
    struct sockaddr_in dest_addr; //структура, хранящая npoint сервера
    struct udphdr *udph; //udp-заголовок
    struct ip *iph; //ip-заголовок
    socklen_t addr_size;
    char *buff = malloc(MSG_SIZE + UDP_HDR_SIZE + IP_HDR_SIZE);
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
    memset(buff, 0, IP_HDR_SIZE + UDP_HDR_SIZE + MSG_SIZE); //зануляем буфер
    printf("enter the message: ");
    msg = buff + IP_HDR_SIZE + UDP_HDR_SIZE;
    write_msg(msg);
    total_size = IP_HDR_SIZE + UDP_HDR_SIZE + strlen(msg);
    
    /* Заполняем IP заголовок */
    iph = (struct ip *)buff;
    iph->ip_hl = 5;
    iph->ip_v = 4;
    iph->ip_len = total_size;
    iph->ip_id = htonl(54321);
    iph->ip_ttl = 255;
    iph->ip_p = IPPROTO_UDP;
    iph->ip_src.s_addr = inet_addr("127.0.0.1");
    iph->ip_dst.s_addr = dest_addr.sin_addr.s_addr;

    /* Заполняем UDP заголовок */
    udph = (struct udphdr *)(buff + sizeof(struct ip));
    udph->source = htons(SOURCE_PORT); 
    udph->dest = htons(DEST_PORT);
    udph->len = htons(UDP_HDR_SIZE + strlen(msg)); 

    setsockopt(sfd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val)); //настройка сокета

    addr_size = sizeof(struct sockaddr_in);
    
    /* Отправляем сообщение серверу */
    if(sendto(sfd, buff, total_size, 0, (struct sockaddr *) &dest_addr, addr_size) == -1){
        close(sfd);
        perror("send error");
        exit(EXIT_FAILURE);
    }

    printf("sent to server: %s\n", buff + UDP_HDR_SIZE + IP_HDR_SIZE);

    recv_size = total_size + MSG_SIZE;

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