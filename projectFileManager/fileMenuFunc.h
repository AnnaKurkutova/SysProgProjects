/* Объявление функций */

/* Печать посередине строки окна*/
void printInMiddle(WINDOW*, int, int, int, char*, chtype);
/* Отрисовка шапки */
void drawHeader(WINDOW*);
/* Инициализация окон */
void initWins(WINDOW**);
/* Работа с директориями */
int dirFirstOpen(WINDOW* ,char*, char**);
int dirOpen(WINDOW* ,char*, char**);
/* Создание и прорисовка окон и панелей */
void makeWins(WINDOW**, PANEL**, char**, char***, int*);
/* Удаление окон */
void destroyWin(WINDOW*);