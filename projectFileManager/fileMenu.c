
#include <stdlib.h>
#include <panel.h> // включает в себя библиотеку <curses.h>
#include <dirent.h> // для работы с директориями
#include <string.h> // для работы со строками
#include <unistd.h>
#include "fileMenuFunc.h"

/************************************************************
**Данная программа является упрощённой версией файлового   **
**менеджера. Пользователю предоставляется возможность      **
**перемещения по каталогам, просмотра внутренних файлов    **
**и каталогов, запуск исполняемых файлов. Для переключения **
**между окнами необходимо нажать Tab, для перемещения      **
**внутри окна - KEY_UP, KEY_DOWN, для открытия следующего  **
**каталога - Enter, для завершения работы программы -      **
**Escape. После запуска исполняемого файла, программа ждёт **
**2 секунды и снова выводит в двух окнах файлы и каталоги  **
**текущих для окон директорий.                             **
************************************************************/

int main(){	
    // Переменные для работы с директориями
    int maxSize = 255; // максимальное количество имён каталогов и файлов в массиве
    int num = 2; // количество окон и панелей
    //char *namePath = malloc(PATH_MAX + 1);// путь
    //strncpy(namePath, "/home", maxSize);// установка изначальной директории
    int nstrok = maxSize, mstolb = PATH_MAX + 1;// количество строк и столбцов массивов
    int numOfElems[num];// количество каталогов и файлов в окне
    int key;// символ с клавиатуры для перемещения по директориям
    int buff = 0;//для проверки количества выводимых на экран файлов

    // Переменные для работы с ncurses
    int header = 3; // отступ под шапку
    int x,y;// координаты окна
    int row, col;// высота и ширина окна
    int flag = 1;// номер окна
    WINDOW *myWins[num];// окна
    PANEL  *myPanels[num];// панели
    PANEL  *top;// указатель на верхнюю панель
    char *oldNamePath;

    // Выделение памяти под массив имён текущего каталога, по одному имени для каждого окна
    char **namePaths;
    namePaths = malloc(num*sizeof(char*));
    for (int i = 0; i < num; i++){
        namePaths[i] = malloc(mstolb);
        strncpy(namePaths[i], "/home", mstolb);
    }

    // Выделение памяти под массив имён каталогов и файлов
    char ***arrOfPathNames;
    arrOfPathNames = malloc(num*sizeof(char**));// для каждого окна свой массив строк

    for (int i = 0; i < num; i++){
        arrOfPathNames[i] = malloc(nstrok*sizeof(char*));

        for(int j = 0; j < nstrok; j++){
            arrOfPathNames[i][j] = malloc(mstolb*sizeof(char));
        }
    }
    
    makeWins(myWins, myPanels, namePaths, arrOfPathNames, numOfElems);
    // Установка указателей на следующую панель
    set_panel_userptr(myPanels[0], myPanels[1]);
    set_panel_userptr(myPanels[1], myPanels[0]);
    top = myPanels[flag];// верхняя панель под индексом 1

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
                if(y > header){
                    mvwchgat(panel_window(top), y, 0, -1, A_NORMAL|A_BOLD, 1, NULL);
                    y--;
                    mvwchgat(panel_window(top), y, 0, -1, A_REVERSE, 0, NULL);
                }
            }
            break;

        // вниз на одну строку
            case KEY_DOWN:{
                getyx(panel_window(top), y, x);
                if(((y - (header - 1)) < numOfElems[flag]) && (y < row-1)){
                    mvwchgat(panel_window(top), y, 0, -1, A_NORMAL|A_BOLD, 1, NULL);
                    y++;
                    mvwchgat(panel_window(top), y, 0, -1, A_REVERSE, 0, NULL);
                }
            }
            break;

        // выбор директории
            case '\n':{
                if((y - header) <= numOfElems[flag]){
                    oldNamePath = malloc(mstolb);
                    strncpy(oldNamePath, namePaths[flag], PATH_MAX);
                    chdir(namePaths[flag]);
                    free(namePaths[flag]);// высвобождаем память под путь
                    namePaths[flag] = malloc(mstolb);// выделяем заново памть под путь
                    strncpy(namePaths[flag], arrOfPathNames[flag][y - header], PATH_MAX);// новый путь

                    //Высвобождение памяти массива каталогов и файлов
                    for (int i = 0; i < nstrok; i++){
                        free(arrOfPathNames[flag][i]);
                    }
                    free(arrOfPathNames[flag]);

                    // Выделение памяти под массив каталогов и файлов
                    arrOfPathNames[flag] = malloc(nstrok*sizeof(char*));
                    for(int i = 0; i < nstrok; i++){
                        arrOfPathNames[flag][i] = malloc(mstolb);
                    }
                    
                    wmove(panel_window(top), 0, 0);// в левый верхний угол окна
                    wclear(panel_window(top));// очистка экрана
                    drawHeader(panel_window(top));// отрисовка шапки

                    buff = dirOpen(panel_window(top), namePaths[flag], arrOfPathNames[flag]);
                    //если открывался файл, то перерисовываем окна и вместо названия файла в namePath помещаем текущий каталог
                    if(buff == 0){
                        free(namePaths[flag]);
                        namePaths[flag] = malloc(mstolb);// выделяем заново памть под путь
                        strncpy(namePaths[flag], ".", PATH_MAX);// новый путь
                        wclear(myWins[0]);
                        wclear(myWins[1]);
                        refresh();
                        makeWins(myWins, myPanels, namePaths, arrOfPathNames, numOfElems);
                        // Установка указателей на следующую панель
                        set_panel_userptr(myPanels[0], myPanels[1]);
                        set_panel_userptr(myPanels[1], myPanels[0]);
                        top = myPanels[flag];// верхняя панель под индексом 1

                        getmaxyx(panel_window(top), row, col);// высота и ширина текущего окна
                        getyx(panel_window(top), y, x);// текущие координаты верхнего окна
                    }
                    //если открывалась директория, то заносим количество элементов на экране в массив
                    else{
                        numOfElems[flag] = buff;
                    }
                    free(oldNamePath);
                }  
            }
            break;
        }
	
	// Обновление и перерисовка
	update_panels();
	doupdate();
    }

    // Уничтожение созданных панелей и окон
    for(int i = 0; i < num; i++){
    	del_panel(myPanels[i]);
    	destroyWin(myWins[i]);
    }

    // Высвобождение памяти 
    for(int i = 0; i < num; i++){
        free(namePaths[i]);
    }
    free(namePaths);
    
    for (int i = 0; i < num; i++){
        for(int j = 0; j < nstrok; j++){
            free(arrOfPathNames[i][j]);
        }
        free(arrOfPathNames[i]);
    }
    free(arrOfPathNames);

    // Завершение программы
    endwin();
    if(isendwin() == FALSE){
        refresh();
    }
    exit(EXIT_SUCCESS);
}

