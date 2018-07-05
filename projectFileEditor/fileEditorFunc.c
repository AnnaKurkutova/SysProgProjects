#include <unistd.h> /* close() */
#include <stdlib.h>
#include <ncurses.h>

/* Чтение имени файла с клавиатуры */
void getPathName(WINDOW *wnd, char *pathName, int maxPathSize){
    int i = 0;
    int ch;
    while (((ch = wgetch(wnd)) != '\n') && (i < maxPathSize)) {
        pathName[i++] = ch;
        wechochar(wnd, ch);
    }
    pathName[i] = '\0';
    wechochar(wnd, '\n');
}

/* Контроль ошибок при открытии файла */
void openError(int fd){
    if (fd == -1){
        perror("Error in opening file");
        exit(EXIT_FAILURE);
    }
}

/* Контроль ошибок при перемещениям по файлу */
void posError(long pos){
    if (pos == -1L){
        perror("lseek to this position failed");
        exit(EXIT_FAILURE);
    }
}

/* Контроль ошибок при закрытии файла */
void closeError(int fd){
    if(close(fd)){
        perror("Error in closing file");
        exit(EXIT_FAILURE);
    }
}

