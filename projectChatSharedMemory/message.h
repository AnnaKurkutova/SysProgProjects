#define MSG_TYPE_STRING 1 //передана строка
#define MAX_TEXT 100 //максимальный размер сообщения
#define MAX_NAME 20 //максимальный размер имени
#define NUM_SIZE 4 //максимальный размер номера операции
#define NUM_MESSAGES 100 //максимальное количество сообщений

/* Cтруктура сообщения, помещаемого в разделяемую память */
typedef struct message_t{
    int type;
    char string[MAX_TEXT];
} message_t;