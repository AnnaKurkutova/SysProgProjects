#define LISTEN_NUM 5
#define BUFF_SIZE 100
#define THREADS_NUM 5
#define MSG_TYPE 1
#define WELCOME_MSG_SIZE 1
#define CHECK 0
#define WRITE 1
#define READ 2
#define ERROR -1
#define SUCCEED 0

/* Структура, определяющая образец сообщения */
struct msg_buff{
    long type;  //тип сообщения 
    int sfd; //дескриптор
    char text[WELCOME_MSG_SIZE]; //сообщение
};

/* Функция контроля ошибок при создании потока */
void errorCreateThread(int);
/* Функция контроля ошибок при ожидании завершения потока */
void errorJoinThread(void*);
/* Функция отправки сообщения в очередь сообщений*/
void send_msg(int, int, char*);
/* Функция получения сообщения из очереди сообщений*/
void receive_msg(int, int*, char*);
/* Функция потока */
void *workWithClient(void*);