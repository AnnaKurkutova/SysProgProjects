#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>// определения констант
#include <errno.h>
#include <panel.h>
#include <dirent.h>// для работы с директориями#include <dirent.h>
#include <string.h>
#include <unistd.h>// для работы со stat()
#include <sys/types.h>// для работы со stat()
#include <sys/stat.h>// для работы со stat()

int num = 2; // количество окон и панелей
int maxLenth = 255; // максимальное количество имён каталогов и файлов в массиве
int header = 3; // отступ под шапку


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

// Первая папка - домашний каталог, это точно каталог
int dirFirstOpen(WINDOW* win ,char *namePath, char **arrOfPathNames){
    int k = 0;// количество строк в массиве

    wmove(win, header, 0);// пропускаем шапку
    
    struct dirent **nameList;
    int n;

    n = scandir(namePath, &nameList, 0, alphasort);// сортировка в алфавитном порядке
    if (n < 0){
        perror("scandir");
    }
    else {
        for(int i = 1; i < n; i++){
            strncpy(arrOfPathNames[k], nameList[i]->d_name, PATH_MAX); 
            wprintw(win, "%s\n", arrOfPathNames[k]);
            k++;
            free(nameList[i]);
        }
        wmove(win, header, 0);
        wrefresh(win);
        free(nameList);
    }
    
    return k;
}
/* Работа с директориями 
Функция возвращает количество строк в массиве, содержащем
файлы и подкаталоги текущего каталога с именем пути namePath*/
int dirOpen(WINDOW* win ,char *namePath, char **arrOfPathNames){
    int k = 0;// количество строк в массиве

    wmove(win, header, 0);// пропускаем шапку
    
    struct stat entryInfo;// информация о файле
    stat(namePath, &entryInfo);
    
    // Если по имени пути находится каталог
    if (S_ISDIR(entryInfo.st_mode)){
        struct dirent **nameList;
        int n;
        //chdir(namePath);
        n = scandir(namePath, &nameList, 0, alphasort);// сортировка в алфавитном порядке
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
            wmove(win, header, 0);
            wrefresh(win);
            free(nameList);
        }
    }
    else if(S_ISREG(entryInfo.st_mode) && (entryInfo.st_mode & S_IXUSR)){//S_IEXEC  
        pid_t pid;
        char buff[PATH_MAX +1];

        strncpy(buff, "./", PATH_MAX);
        strncat(buff, namePath, PATH_MAX);
        pid = fork();
        //дочерний процесс
        if(pid == 0){
            if(execlp(buff, buff, NULL) == -1){
                perror("Error in executing the file\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(pid == -1){
            perror("Error in creating the process\n");
            exit(EXIT_FAILURE);
        }
        sleep(2);
    }
    return k;
}

void makeWins(WINDOW **myWins, PANEL **myPanels, char **namePaths, char ***arrOfPathNames, int *numOfElems){
    // Инициализация ncurses 
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);// активация работы с символами клавиатуры
    curs_set(FALSE);// скрытие курсора

    // Инициализация цветовой палитры
    if (!has_colors()){
        endwin();
        printf("\nError!\n");// если не поддерживаются цвета
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    refresh();

    // Инициализация окон и панелей
    initWins(myWins);
    for(int i = 0; i < num; i++){
        myPanels[i] = new_panel(myWins[i]);// создание панелей на основе созданных окон
        numOfElems[i] = dirFirstOpen(myWins[i], namePaths[i], arrOfPathNames[i]);// вывод в окно состава первой диреткории
    }
    // Обновление и прорисовка
    update_panels();
    mvprintw(0, 0, "   Tab - next panel  Escape - exit");
    refresh();
    doupdate();
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
