/* Объявление функций */

/* Функция выводит идентификатор потока и завершает его */
void *printNum(void*);

/* Функция контроля ошибок при создании потока */
void errorCreateThread(int);

/* Функция контроля ошибок при ожидании завершения потока */
void errorJoinThread(void*);