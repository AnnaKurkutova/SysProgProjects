#include <stdlib.h>
#include <panel.h>
#include <dirent.h>// для работы с директориями
#include <string.h>
#include "fileMenuFunc.h"


int main()
{	
    /* Переменные для работы с директориями*/
    const int maxLenth = 255;// максимальный размер пути
    char *namePath = (char *)malloc(maxLenth + 1);// путь
    strncpy(namePath, "/home/", maxLenth);// домашняя папка
    char *str=(char *)malloc(PATH_MAX + 1);// имя текущего каталога
    int key;// символ с клавиатуры для перемещения по директориям

    /* Выделение памяти под массив путей каталогов и файлов  */
    char **arrOfPathNames;
    int nstrok = maxLenth, mstolb = PATH_MAX + 1;

    arrOfPathNames = (char**)malloc(nstrok*sizeof(char*));

    for(int i = 0;i < nstrok;i++){
        arrOfPathNames[i]=(char*)malloc(mstolb*sizeof(char));
    }
    
    /* Переменные для работы с ncurses*/
    WINDOW *my_wins[2];// окна
    PANEL  *my_panels[2];// панели
    PANEL  *top;

    /* Инициализация ncurses */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    /* Инициализация цветовой палитры */
    if (!has_colors()){
    	endwin();
    	printf("\nError!\n");// если не поддерживаются цвета
    	return 1;
    }
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);

    /* Инициализация окон */
    init_wins(my_wins);
	
    /* Создание панелей на основе созданных окон */
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    
    /* Вывод в окна состава первой директории*/
    //dirOpen(my_wins[0], namePath);
    //dirOpen(my_wins[1], namePath);
    int k;
    k = dirOpen(my_wins[0], namePath, arrOfPathNames, maxLenth);
    k = dirOpen(my_wins[1], namePath, arrOfPathNames, maxLenth);


    /* Установка указателей на следующее окно
    для перехода при нажатии Tab на следующее окно*/
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[0]);

    /* Обновление и прорисовка */ 
    update_panels();
    mvprintw(0, 0, "   Tab - next panel  Escape - exit");
    doupdate();

    /*Т.к. панель с индексом 1 создавалась последней, 
    она и будет верхней */
    top = my_panels[1];
    
    int i = 1;// номер окна
    int x,y;// координаты окна
    getyx(my_wins[1], y, x);// текущие координаты верхнего окна

    /* Цикл обработки сообщений с клавиатуры */
    while((key = getch()) != '\e')
    {	
    	switch(key){
        // переход на следующую панель
    	    case '\t': {
    			top = (PANEL *)panel_userptr(top);
    			top_panel(top);
                if(i == 1) i = 0;
                else i = 1;
            }
            break;

        // вверх на одну строку
            case KEY_UP:{
                getyx(my_wins[i], y, x);
                y--;
                wmove(my_wins[i], y, x);
            }
            break;

        // вниз на одну строку
            case KEY_DOWN:{
                getyx(my_wins[i], y, x);
                y++;
                wmove(my_wins[i], y, x);
            }
            break;

        // выбор директории
            case '\n':{
                if(y <= k){
                    strncat( namePath, arrOfPathNames[y], maxLenth);
                    strncat( namePath, "/", maxLenth);
                    wmove(my_wins[i], 0, 0);
                    wclear(my_wins[i]);// очистка экрана
                    k = dirOpen(my_wins[i], namePath, arrOfPathNames, maxLenth);
                }
            }
            break;
        }
	
	/* Обновление и перерисовка */ 
	update_panels();
	doupdate();
    }

    /* Уничтожение созданных панелей и окон */ 
    for(int j = 0; j < 2; ++j)
    {
    	del_panel(my_panels[j]);
    	delwin(my_wins[j]);
    }

    /* Высвобождение памяти */
    free(str);
    free(namePath);

    for (int j = 0; j < maxLenth; ++j){
        free(arrOfPathNames[j]);
    }

    free(arrOfPathNames);

    /* Завершение программы */
    endwin();
    return 0;
}

