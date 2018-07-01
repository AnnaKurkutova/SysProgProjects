#include <stdlib.h>
#include <panel.h>
#include <dirent.h>// для работы с директориями#include <dirent.h>
#include <string.h>
#include <unistd.h>// для работы со stat()
#include <sys/types.h>// для работы со stat()
#include <sys/stat.h>// для работы со stat()

// Печать текста посередине строки окна
void printInMiddle(WINDOW *win, int starty, int startx, int width, char *string, chtype color){
    int length, x, y;
    float temp;

    if(win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if(startx != 0)
        x = startx;
    if(starty != 0)
        y = starty;
    if(width == 0)
        width = 80;

    length = strlen(string);
    temp = (width - length)/ 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    wrefresh(win);
}

/* Отрисовка шапки */
void drawHeader(WINDOW *myWin){
    int x, y, raw, col;
    char* label = "Name of file or directory";

    getbegyx(myWin, y, x);// текущие координаты окна
    getmaxyx(myWin, raw, col);// высота и ширина окна
 
    printInMiddle(myWin, 1, 0, col, label, COLOR_PAIR(1));
    mvwhline(myWin, 2, 0, ACS_HLINE, col); 
    wmove(myWin, 3, 0);
}

/* Инициализация окон */
void initWins(WINDOW **myWins){
    int x = 1, y = 1;// координаты относительно главного окна
    int row, col;//высота и ширина окна 
    int n = 2;// количество окон

    getmaxyx(stdscr, row, col);// размеры экрана
    row-=2;
    col=col/2 - 1.5;

    for(int i = 0; i < n; ++i){
        myWins[i] = newwin(row, col, y, x);// инициализация окна
        wbkgdset(myWins[i], COLOR_PAIR(1));// установка цветовой пары
        wattron(myWins[i], A_BOLD);// жирный шрифт текста
        wclear(myWins[i]);// очистка экрана
        wrefresh(myWins[i]);// обновление экрана
        drawHeader(myWins[i]);
 
        x += col + 1;// сдвиг координаты x
    }
} 

/* Работа с директориями 
Функция возвращает количество строк в массиве, содержащем
файлы и подкаталоги текущего каталога с именем пути namePath*/
int dirOpen(WINDOW* win ,char* namePath, char **arrOfPathNames, int maxLenth, int header){
    int nstrok = maxLenth, mstolb = PATH_MAX + 1;// количество строк и столбцов
    int k = 0;// количество строк в массиве
    int result;

    // Заходим в каталог
    result = chdir(namePath);

    wmove(win, header, 0);// пропускаем шапку
    
    struct stat entryInfo;// информация о файле
    stat( ".", &entryInfo );
    
    // Если по имени пути находится каталог
    if ((entryInfo.st_mode & S_IFMT) == S_IFDIR){
        struct dirent **nameList;
        int n;

        n = scandir(".", &nameList, 0, alphasort);// сортировка в алфавитном порядке
        if (n < 0){
            perror("scandir");
        }
        else {
            for(int i = 1; i < n; i++) {
                    strncpy(arrOfPathNames[k], nameList[i]->d_name, PATH_MAX); 
                    wprintw(win, "%s\n", arrOfPathNames[k]);
                    k++;
                    free(nameList[i]);
                }
            }
            wmove(win, header, 0);
            wrefresh(win);
            free(nameList);
        }
    return k;
}

void destroyWin(WINDOW *myWin)
{       
        wborder(myWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        /* Принимаемые параметры: 
         * 1. myWin: текущее окно
         * 2. ls: левый край окна
         * 3. rs: правый край окна
         * 4. ts: верхний край окна
         * 5. bs: нижний край окна
         * 6. tl: левый верхний угол окна 
         * 7. tr: правый верхний угол окна 
         * 8. bl: левый нижний угол окна 
         * 9. br: правый нижний угол окна
         */
        wrefresh(myWin);
        delwin(myWin);
}
