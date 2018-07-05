/* Объявление функций */

/* Чтение имени файла с клавиатуры */
void getPathName(WINDOW*, char*, int);
/* Контроль ошибок при открытии файла */
void openError(int);
/* Контроль ошибок при перемещениям по файлу */
void posError(long);
/* Контроль ошибок при закрытии файла */
void closeError(int);