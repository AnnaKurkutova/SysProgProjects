/* Объявление функций */

/* Инициализация окон */
void init_wins(WINDOW**);
/* Проверка на наличие точек в названии */
int isItFile(char*);
/* Работа с директориями */
int dirOpen(WINDOW* ,char* , char**, int);