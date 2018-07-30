#define LISTEN_NUM 5
#define PORT_NUM 2018
#define ROW 24
#define COL 80
#define TIMEOUT 10
#define CLIENTS_NUM 16
#define MSG_SIZE 100

/* Данные для передачи в функцию потока */
struct thread_data{
    int broadcast_fd;
    struct sockaddr_in udp_address;
    struct sockaddr_in udp_bc_address;
};
