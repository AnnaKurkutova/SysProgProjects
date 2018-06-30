/* Объявление функций */

/* Печать посередине строки окна*/
void printInMiddle(WINDOW*, int, int, int, char*, chtype);
/* Отрисовка шапки */
void drawHeader(WINDOW*);
/* Инициализация окон */
void initWins(WINDOW**);
/* Работа с директориями */
int dirOpen(WINDOW* ,char* , char**, int, int);