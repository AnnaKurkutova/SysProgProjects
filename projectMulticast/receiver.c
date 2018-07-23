#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>

/*********************************************************************************
*  Адреса IPv4 Class D (то есть со значениями в диапазоне от 224.0.0.0 до        *
*  239.255.255.255) зарезервированы для многоадресных рассылок. Каждый адрес     *
*  может быть связан с группой многоадресной рассылки. Чтобы получить данные,    *
*  переданные по этому адресу, необходимо стать членом такой группы. Для         *
*  подключения к группе используется метод setsockopt(). Функция setsockopt()    *
*  вызывается со следующими параметрами: sfd - дескриптор сокета, IPPROTO_IP     *
*  - уровень параметров сокета для IP, IP_ADD_MEMBERSHIP - опция добавления в    *
*  группу, &mreq - ссылка на структуру с адресом мультикастовой группы и         *
*  адресом локального интерфейса, sizeof(struct ip_mreqn) - размер структуры.    *
*  INADDR_ANY в данном случае говорит о том, что мы оставляем право выбора       *
*  интерфейса за ядром. Далее ядро проверит не подключены ли мы уже к этой       *
*  группе, и если нет, то отправит запрос на ближайший Multicast сервер.         *
*********************************************************************************/

#define BUFF_SIZE 100

void main(){
	int sfd; //дескриптор сокета
    struct sockaddr_in send_addr; //структура, хранящая npoint
    struct ip_mreqn mreq; //структура для присоединения к мультикастовой группе
    socklen_t addr_size; //размер структуры
    char buff[BUFF_SIZE]; //строка с сообщением

    /* Инициализируем дескриптор сокета */
    if((sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Задаем адрес и порт */
    memset(&send_addr, 0, sizeof(struct sockaddr_in));
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(7777);
    send_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&mreq, 0, sizeof(struct ip_mreqn));
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.1"); //адрес мультикастовой группы
    mreq.imr_address.s_addr = htonl(INADDR_ANY); //адрес локального интерфейса

    /* Присоединение к мультикастовой группе */
    setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreqn));

    /* Связываем адрес с дескриптором сокета */
    if((bind(sfd, (struct sockaddr *) &send_addr, sizeof(struct sockaddr_in))) == -1){
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    addr_size = sizeof(struct sockaddr_in); //размер структуры

    /* Принимаем сообщение */
    if(recvfrom(sfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &send_addr, &addr_size) == -1){
        close(sfd);
        perror("recvfrom error");
        exit(EXIT_FAILURE);
    }
    printf("received: %s\n", buff);

    close(sfd);
    exit(EXIT_SUCCESS);
}