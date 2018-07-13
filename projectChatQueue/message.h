#define TEXT_SIZE 100
#define NAME_SIZE 10
#define NUM_SIZE 10
#define MSG_TYPE 1

/* Структура, определяющая образец сообщения */
struct msg_buff{
    long type;  //тип сообщения 
    char text[TEXT_SIZE]; //сообщение
};

/* Объявление функций */
void send_msg(int, char*); //функция отправки сообщения в очередь
void receive_msg(int, char*); //функция чтения сообщения из очереди