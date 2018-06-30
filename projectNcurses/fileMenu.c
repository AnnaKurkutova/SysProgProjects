/*******************************************************
*Данная программа является упрощённой версией файлового*
*менеджера. Пользователю предоставляется возможность   *
*перемещения по каталогам, просмотра внутренних файлов *
*и каталогов. Для переключения между окнами необходимо *
*нажать Tab, для перемещения внутри окна - KEY_UP,     *
*KEY_DOWN, для открытия следующего каталога - Enter,   *
*для завершения работы программы - Escape.             *
*******************************************************/

#include <stdlib.h>
#include <panel.h>// включает в себя библиотеку <curses.h>
#include <dirent.h>// для работы с директориями
#include <string.h>// для работы со строками
#include "fileMenuFunc.h"

#define NUM 2// количество окон и панелей
#define MAX_SIZE 255// максимальное количество имён каталогов и файлов в массиве
#define HEADER 3// отступ под шапку

int main(){	
    // Переменные для работы с директориями
    char *namePath = (char *)malloc(PATH_MAX + 1);// путь
    strncpy(namePath, "/home", MAX_SIZE);// установка изначальной директории
    int nstrok = MAX_SIZE, mstolb = PATH_MAX + 1;// количество строк и столбцов массивов
    int numOfElems[NUM];// количество каталогов и файлов в окне
    int key;// символ с клавиатуры для перемещения по директориям

    // Переменные для работы с ncurses
    int x,y;// координаты окна
    int row, col;// высота и ширина окна
    int flag = 1;// номер окна
    WINDOW *myWins[NUM];// окна
    PANEL  *myPanels[NUM];// панели
    PANEL  *top;// указатель на верхнюю панель

    // Выделение памяти под массив путей каталогов и файлов 
    char ***arrOfPathNames;
    arrOfPathNames = (int ***) malloc(NUM*sizeof(int**));// для каждого окна свой массив строк

    for (int i = 0; i < NUM; i++) {
        arrOfPathNames[i] = (int **) malloc(nstrok*sizeof(int*));

        for(int j = 0; j < nstrok; j++) {
            arrOfPathNames[i][j] = (int *) malloc(mstolb*sizeof(int));
        }
    }

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
    	return 1;
    }
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    refresh();

    // Инициализация окон и панелей
    initWins(myWins);
    for(int i = 0; i < NUM; i++){
        myPanels[i] = new_panel(myWins[i]);// создание панелей на основе созданных окон
        numOfElems[i] = dirOpen(myWins[i], namePath, arrOfPathNames[i], MAX_SIZE, HEADER);// вывод в окна состава первой диреткории
    }
    
    // Установка указателей на следующую панель
    set_panel_userptr(myPanels[0], myPanels[1]);
    set_panel_userptr(myPanels[1], myPanels[0]);
    top = myPanels[flag];// верхняя панель под индексом 1

    // Обновление и прорисовка
    update_panels();
    mvprintw(0, 0, "   Tab - next panel  Escape - exit");
    refresh();
    doupdate();
    
    getmaxyx(panel_window(top), row, col);// высота и ширина текущего окна
    getyx(panel_window(top), y, x);// текущие координаты верхнего окна

    // Цикл обработки сообщений с клавиатуры
    while((key = getch()) != '\e')
    {	
    	switch(key){

        // переход на следующую панель
    	    case '\t': {
                mvwchgat(panel_window(top), y, 0, -1, A_NORMAL|A_BOLD, 1, NULL);// убираем подсветку предыдущей строки
    			top = (PANEL *)panel_userptr(top);
    			top_panel(top);
                getyx(panel_window(top), y, x);
                mvwchgat(panel_window(top), y, 0, -1, A_REVERSE, 0, NULL);// добавляем подсветку текущей строки
                if(flag == 1) flag = 0;
                else flag = 1;
            }
            break;

        // вверх на одну строку
            case KEY_UP:{
                getyx(panel_window(top), y, x);
                if(y > HEADER){
                    mvwchgat(panel_window(top), y, 0, -1, A_NORMAL|A_BOLD, 1, NULL);
                    y--;
                    mvwchgat(panel_window(top), y, 0, -1, A_REVERSE, 0, NULL);
                }
            }
            break;

        // вниз на одну строку
            case KEY_DOWN:{
                getyx(panel_window(top), y, x);
                if(((y - (HEADER - 1)) < numOfElems[flag]) && (y < row-1)){
                    mvwchgat(panel_window(top), y, 0, -1, A_NORMAL|A_BOLD, 1, NULL);
                    y++;
                    mvwchgat(panel_window(top), y, 0, -1, A_REVERSE, 0, NULL);
                }
            }
            break;

        // выбор директории
            case '\n':{
                if((y - HEADER) <= numOfElems[flag]){
                    free(namePath);// высвобождаем память под путь
                    namePath = (char *)malloc(PATH_MAX + 1);// выделяем заново памть под путь
                    strncpy(namePath, arrOfPathNames[flag][y - HEADER], PATH_MAX);// новый путь

                    //Высвобождение памяти массива каталогов и файлов
                    for (int i = 0; i < MAX_SIZE; i++){
                        free(arrOfPathNames[flag][i]);
                    }
                    free(arrOfPathNames[flag]);

                    // Выделение памяти под массив каталогов и файлов
                    arrOfPathNames[flag] = (char**)malloc(nstrok*sizeof(char*));
                    for(int i = 0; i < nstrok; i++){
                        arrOfPathNames[flag][i] = (char*)malloc(mstolb*sizeof(char));
                    }
                    
                    wmove(panel_window(top), 0, 0);// в левый верхний угол окна
                    wclear(panel_window(top));// очистка экрана
                    drawHeader(panel_window(top));// отрисовка шапки
                    numOfElems[flag] = dirOpen(panel_window(top), namePath, arrOfPathNames[flag], MAX_SIZE, HEADER);
                }  
            }
            break;
        }
	
	// Обновление и перерисовка
	update_panels();
	doupdate();
    }

    // Уничтожение созданных панелей и окон
    for(int i = 0; i < NUM; i++){
    	del_panel(myPanels[i]);
    	delwin(myWins[i]);
    }
    
    // Высвобождение памяти 
    for (int i = 0; i < NUM; i++){
        free(arrOfPathNames[i]);

        for(int j = 0; j < nstrok; j++){
            free(arrOfPathNames[i][j]);
        }
    }
    free(arrOfPathNames);
    free(namePath);

    // Завершение программы
    endwin();
    return 0;
}

