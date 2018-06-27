#include <stdlib.h>
#include <panel.h>
#include <dirent.h>// для работы с директориями
#include <string.h>

/* Инициализация окон */
void init_wins(WINDOW **wins)
{

    int x = 1, y = 1;// координаты относительно главного окна
    int row, col;//высота и ширина окна 
    int n = 2;// количество окон

    getmaxyx(stdscr, row, col);// размеры экрана
    row-=2;
    col=col/2 - 1.5;

    for(int i = 0; i < n; ++i)
    {
        wins[i] = newwin(row, col, y, x);// инициализация окна
        wbkgdset(wins[i], COLOR_PAIR(1));// установка цветовой пары
        wattron(wins[i], A_BOLD);// жирный шрифт текста
        wclear(wins[i]);// очистка экрана
        wrefresh(wins[i]);// обновление экрана
 
        x += col + 1;// сдвиг координаты x
    }
} 

/* Проверка на наличие точек в названии */
int isItFile(char* name){
    int flag = 1;
    int nameLenth = strlen(name);

    for(int i = 0; i < nameLenth; i++){
        if(name[i] == '.'){
            flag = 0;
            break;
        }
    }

    return flag;
}

/* Работа с директориями 
Функция возвращает количество строк в массиве, который содержит
файлы и подкаталоги текущего каталога с именем пути namePath*/
int dirOpen(WINDOW* win ,char* namePath, char **arrOfPathNames, int maxLenth){
    int nstrok = maxLenth, mstolb = PATH_MAX + 1;// количество строк и столбцов

    /* Высвобождение памяти */
    for (int j = 0; j < maxLenth; ++j){
        free(arrOfPathNames[j]);
    }
    free(arrOfPathNames);

    /* Выделение памяти */
    arrOfPathNames = (char**)malloc(nstrok*sizeof(char*));
    for(int i = 0;i < nstrok;i++){
        arrOfPathNames[i]=(char*)malloc(mstolb*sizeof(char));
    }

     /* Открытие директории */
    DIR *dirBuf = NULL;
    struct dirent *entry;
    
    dirBuf = opendir(namePath);//открытие директории для чтения

    int k = 0;// количество строк в массиве

    if(dirBuf != NULL)
    {
        while(((entry = readdir(dirBuf)) != NULL) && (k != (maxLenth -1)) ){
            int flag = isItFile(entry->d_name);// не выводим файлы
            if(flag == 1){
                strncpy(arrOfPathNames[k], entry->d_name, PATH_MAX); 
                wprintw(win, "%s\n", arrOfPathNames[k]);
                k++;
            }
        }
        strncpy(arrOfPathNames[k], "..", PATH_MAX);//последним выводится ..
        wprintw(win, "%s\n", arrOfPathNames[k]);   
    }

    return k;
}
