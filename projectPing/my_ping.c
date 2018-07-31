#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/******************************************
*  Ping Algorithm                         *
*  1) Initialize echo request             *
*  2) Send echo request                   *
*  3) Wait for echo reply (or time out)   *
*  4) Receive reply                       *
*  5) Report results                      *
******************************************/

#define MSG_SIZE 1000
#define IP_HDR_SIZE 20
#define PORT_NUM 7777

/* Checksum calculating */
unsigned short check_sum (unsigned short *buff, int total_size) {
    unsigned long sum;
    u_short oddbyte;
    unsigned short result;
 
    sum = 0;

    while (total_size > 1) {
        sum += *buff++;
        total_size -= 2;
    }
 
    if (total_size == 1) {
        oddbyte = 0;
        *((u_char *) & total_size) = *(u_char *) buff;
        sum += total_size;
    }
 
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    result = (short) ~sum;
 
    return result;
}

void main(){
    struct sockaddr_in dest_addr;
    struct icmphdr *icmph_request; //icmp header
    struct icmphdr *icmph_reply;
    socklen_t addr_size;
    char *buff;
    char *msg;
    int sfd;
    int icmph_size; //size of icmp header
    int total_size; //total size = icmp header size + message size
    int reply_size; //size of echo reply
    int sequence_num = 1;
    int id_num = 1;


    /* Setting up IP address and port number */
    memset(&dest_addr, 0, sizeof(struct sockaddr_in));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT_NUM);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    addr_size = sizeof(struct sockaddr_in);

    /* Raw socket initialization */
    if((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1){
        perror("error in creating socket");
        exit(EXIT_FAILURE);
    }

    /* Creating the message buffer and filling it in */
    icmph_size = sizeof(struct icmphdr);
    buff = malloc(icmph_size + MSG_SIZE); //memory allocation
    memset(buff, 0, icmph_size + MSG_SIZE); //buff nullification
    icmph_request = (struct icmphdr *)buff; //icmp header will be in the beggining of buff
    msg = buff + icmph_size; //message will be after icmp header
    strcpy(msg, "echo request");
    total_size = strlen(msg) + icmph_size; //real total size

    /* ICMP header filling */
    icmph_request->type = ICMP_ECHO;
    icmph_request->code = 0; //zero for ICMP echo and reply messages
    icmph_request->un.echo.id = htons(sequence_num);
    icmph_request->un.echo.sequence = htons(sequence_num);
    icmph_request->checksum = check_sum((unsigned short *)buff, total_size); //calculated as in IP header

    /* Sending echo request */
    if(sendto(sfd, buff, total_size, 0, (struct sockaddr *) &dest_addr, addr_size) == -1){
        close(sfd);
        free(buff);
        perror("send error");
        exit(EXIT_FAILURE);
    }

    while(1) {
    	free(buff);
        buff = malloc(MSG_SIZE);

        /* Receiving echo reply */
        if((reply_size = recvfrom(sfd, buff, MSG_SIZE, 0, 0, 0)) == -1){
            close(sfd);
            free(buff);
            perror("recv error");
            exit(EXIT_FAILURE);
        }

        reply_size -= IP_HDR_SIZE; //echo reply includes IP header
        icmph_reply = (struct icmphdr *)(buff + IP_HDR_SIZE);
        
        //Check if the response is an echo reply, transaction ID and sequence number 
        //are same as for the request
        if((icmph_reply->type == 0) && 
        	(ntohs(icmph_reply->un.echo.id) == id_num) && 
        	(ntohs(icmph_reply->un.echo.sequence) == sequence_num)){
            printf("All's OK\n");
            break;
        }
    }
    
    free(buff);
    close(sfd);
    exit(EXIT_SUCCESS);
}