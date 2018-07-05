#include <fcntl.h>  /* open() and O_XXX flags */
#include <unistd.h> /* close() */
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "fileEditorFunc.h"

/**************************************************************
**Данная программа является примитивным текстовым редактором.**
**В функционал входит: открытие/закрытие файла с сохранением **
**изменений, перемещение по файлу влево/вправо, вниз/вверх   **
**(относительно размеров экрана), выбор следующего файла.    **
**Чтобы открыть файл, пользователю необходимо ввести путь до **
**файла. Используются следующие клавиши для тестирования     **
**работы программы: Escape - выход, KEY_UP, KEY_DOWN,        **
**KEY_LEFT, KEY_RIGHT - перемещение по экрану и одновременно **
**по файлу, Enter - внесение изменений, выход из режима      **
**изменения также Escape, Tab - смена файла.                 **
**Для работы с файлами используются функции open(),close(),  **
**write(), read(), lseek().                                  **
**************************************************************/

int main(void){
    int fd; //файловый дескриптор
    int row, col; //количество строк и столбцов на экране терминала
    int y, x; //текущие координаты экрана
    int yboard, xboard; //границы файла на экране
    int ch; //символ с клавиатуры для выбора операций в меню
    int maxPathSize = 100; //максимальная длина пути
    int maxBuffSize = 100; //максимальный размер массива buff
    size_t bytesRead; //считываемые с файла байты
    long pos; //позиция в файле 
    char *arrText; //текст из файла
    char *buff = malloc(maxBuffSize + 1); //текст для записи в файл
    char *pathName = malloc(maxPathSize + 1); //имя пути
    WINDOW *wnd; //окно


    //инициализация ncurses 
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    //инициализация цветовой пары
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    //считывание размеров экрана
    getmaxyx(stdscr, row, col);
    wnd = newwin(row, col, 0, 0);// создание нового окна с размерами (максимум по y)-1 и максимум по x
    wbkgd(wnd, COLOR_PAIR(1));// выбор цветовой пары
    wattron(wnd, A_BOLD);// жирный шрифт
    //определение размеров страницы
    int PAGE_SIZE = row * col;
    arrText = malloc(PAGE_SIZE + 1);

    wprintw(wnd, "After selecting a file you can use:\n");
    wprintw(wnd, "<Escape> to exit,\n");
    wprintw(wnd, "<Tab> to change the file,\n");
    wprintw(wnd, "<Enter> to change the file and <Escape> to save changes,\n");
    wprintw(wnd, "File navigation: <KEY_UP>, <KEY_DOWN>, <KEY_LEFT>, <KEY_RIGHT>.\n");
    wprintw(wnd, "Enter file path and press <Enter>...\n");
    refresh();
    getPathName(wnd, pathName, maxPathSize); //считывание имени файла с клавиатуры
    //открытие файла на чтение и запись
    fd = open(pathName, O_RDWR, 0);
    openError(fd);
    //чтение файла в массив
    do{
        bytesRead = read(fd, arrText, PAGE_SIZE);
    } while (bytesRead == PAGE_SIZE); 
    
    wclear(wnd); //очистка экрана
    mvwaddnstr(wnd, 0, 0, arrText, PAGE_SIZE); //вывод содержимого файла на экран
    getyx(wnd, yboard, xboard);
    wmove(wnd, 0, 0); //перемещение в левый верхний угол экрана
    lseek(fd, 0, 0); //перемещение в начало файла
    wrefresh(wnd);
    refresh();
    
    // цикл обработки сообщений с клавиатуры
    while((ch = getch()) != '\e'){   
        switch(ch){
            // вправо на одну позицию
            case KEY_RIGHT:{
                getyx(wnd, y, x);
                if((x < (col-1) && y != yboard) || (x < xboard && y == yboard)){
                    pos = lseek(fd, 1, 1);
                    posError(pos);
                    x++;
                    wmove(wnd, y, x); 
                }                
            }
            break;
            // влево на одну позицию
            case KEY_LEFT:{
                getyx(wnd, y, x);
                if(x > 0){
                    pos = lseek(fd, -1, 1);
                    posError(pos);
                    x--;
                    wmove(wnd, y, x);
                }
            }
            break;
            // вверх на одну позицию
            case KEY_UP:{
                getyx(wnd, y, x);
                if(y > 0){
                    pos = lseek(fd, -(x+1), 1);
                    posError(pos);
                    y--;
                    x = col-1;
                    wmove(wnd, y, x); 
                }                
            }
            break;
            // вниз на одну позицию
            case KEY_DOWN:{
                getyx(wnd, y, x);
                if(y < yboard){
                    pos = lseek(fd, col-x, 1);
                    posError(pos);
                    y++;
                    x = 0;
                    wmove(wnd, y, x);
                }
            }
            break;
            // вставить текст
            case '\n':{
                free(buff);
                buff = malloc(maxBuffSize + 1);
                int i = 0;
                while(((ch = wgetch(wnd)) != '\e') && (i < maxBuffSize-1)){
                    buff[i++] = ch;
                    wechochar(wnd, ch);
                }
                buff[i] = '\0';
                write(fd, buff, i); //запись текста в файл с текущей позиции
                refresh();
            }
            break;
            // выбрать новый файл
            case '\t':{
                closeError(fd);
                free(arrText);
                arrText = malloc(PAGE_SIZE + 1);
                free(pathName);
                pathName = malloc(maxPathSize + 1);
                wclear(wnd);
                refresh();
                wprintw(wnd, "Enter file path...\n");
                refresh();
                getPathName(wnd, pathName, maxPathSize);
                fd = open(pathName, O_RDWR, 0);
                openError(fd);
                do{
                    bytesRead = read(fd, arrText, PAGE_SIZE);
                } while (bytesRead == PAGE_SIZE); 
                wclear(wnd);
                refresh();
                mvwaddnstr(wnd, 0, 0, arrText, PAGE_SIZE);
                getyx(wnd, yboard, xboard);
                refresh();
                wmove(wnd, 0, 0);
                lseek(fd, 0, 0);
            }
            break;
        }
    // обновление и перерисовка
    refresh();
    wrefresh(wnd);
    }
    
    //освобождение памяти
    free(arrText);
    free(buff);
    free(pathName);
    closeError(fd);
    endwin();
    exit(EXIT_SUCCESS);
}
