/**********************************************************************
*Данная программа является калькулятором целых чисел, в котором       *
*доступны операции сложения, вычитания, умножения и деления.          *
*Для реализации динамической загрузки используется интерфейс          *
*динамической загрузки (Dynamic Loading API).                         *
*Используемые функции интерфейса: dlopen, dlsym, dlclose.             *
**********************************************************************/

#include <stdio.h>
#include <dlfcn.h>// для реализации динамической загрузки
#include <malloc.h>// для работы с динамической памятью (функции malloc(), realloc(), free())
#include <sys/types.h>// для работы с директориями
#include <dirent.h>// для работы с директориями
#include <string.h>// для работы со строками
#include "intNumCalcFunc.h"

/*Данная функция вызывает методы по адресу из их библиотек.*/
void funcFromLib(char *funcLib, char *func, int firstValue, int secondValue){
    void *dlHandle;// обработчик библиотеки
    int (*funcPtr)(int, int);// переменная для хранения адреса функции

    // загрузка библиотеки 
    dlHandle = dlopen(funcLib, RTLD_NOW);
    // поиск адреса функции в библиотеке
    funcPtr = dlsym(dlHandle, func);	
    // вызов функции по найденному адресу и вывод результата
    printf("Результат равен %d\n", (*funcPtr)(firstValue, secondValue));
    // отсоединение библиотеки от программы
    dlclose(dlHandle);
}

int main (void){
    int firstValue;// первое число
    int secondValue;// второе число
    int num;// номер операции
    int flag = 1;// показатель продолжения(flag == 1)/завершения(flag == 0) цикла

    char *firstValStr = (char*)malloc(10);// строка с первым числом
    char *secondValStr = (char*)malloc(10);// строка со вторым числом
    char *flagStr = (char*)malloc(3);// показатель продолжения 
    char *numStr = (char*)malloc(3);// номер операции

    char pathName[9] = "./plugin";// путь до каталога
    char arrOfPathNames[4][PATH_MAX + 1];// массив путей до библиотек

    DIR *dirBuf = NULL;
    struct dirent *entry;
    
    //открытие директории для чтения
    dirBuf = opendir(pathName);
    
    /*Функция strncmp() сравнивает строки и используется, чтобы игнорировать элементы ." и "..". 
    Функция strncpy() копирует одну строку в другую.
    Функция strncat() объединяет две строки в одну.
    Функция readdir() возвращает следующую структуру dirent, считанную из файла-директории.
    d_name - имя файла.*/
    if(dirBuf != NULL)
    {
    	int i = 3;
        while((entry = readdir(dirBuf)) != NULL){
            if(( strncmp( entry->d_name, ".", PATH_MAX ) != 0 ) && ( strncmp( entry->d_name, "..", PATH_MAX ) != 0 ) && i >= 0){
        	    printf("%s\n",entry->d_name);
                strncpy( arrOfPathNames[i], pathName, PATH_MAX );
                strncat( arrOfPathNames[i], "/", PATH_MAX );
                strncat( arrOfPathNames[i], entry->d_name, PATH_MAX );	
                i--;
        	}
        }
            
    }
    
    // закрытие директории
    closedir(dirBuf);

    /*Меню выбора операций над целыми числами.*/
    do{
        //ввод с клавиатуры первого числа
        printf("Введите первое число\n");
        fgets(firstValStr, sizeof(firstValStr), stdin);
        sscanf(firstValStr, "%d", &firstValue);

        //ввод с клавиатуры второго числа
        printf("Введите второе число\n");
        fgets(secondValStr, sizeof(secondValStr), stdin);
        sscanf(secondValStr, "%d", &secondValue);

        //выбор операции
        printf("Выберете операцию из списка:\n1.Сложение\n2.Вычитание\n3.Умножение\n4.Деление\n");
        fgets(numStr, sizeof(numStr), stdin);
        sscanf(numStr, "%d", &num);

        switch(num){
            case 1: { 
                //операция поиска суммы целых чисел	
                funcFromLib(arrOfPathNames[0], "addIntegers", firstValue, secondValue);
            }
            break;

            case 2: {
                //операция поиска разности целых чисел
                funcFromLib(arrOfPathNames[1], "subtractIntegers", firstValue, secondValue);
            }
            break;

            case 3: {
                //операция поиска произведения целых чисел
                funcFromLib(arrOfPathNames[2], "multiplyIntegers", firstValue, secondValue);
            }
            break;

            case 4: {
                //операция поиска частного целых чисел
                funcFromLib(arrOfPathNames[3], "divideIntegers", firstValue, secondValue);
            }
            break;

        /*в случае ввода неверного номера операции происходит 
        выход из цикла без осуществления каких-либо вычислений*/
        default: printf("Нет операции с таким номером.\n");
        break;
        }

        //продолжение или завершение вычислений
        printf("Хотите продолжить вычисления?Да-1.Нет-0.\n");
        fgets(flagStr, sizeof(flagStr), stdin);
        sscanf(flagStr, "%d", &flag);

}while(flag == 1);

    //высвобождение памяти
    free(firstValStr);
    free(secondValStr);
    free(flagStr);
    free(numStr);
 
    return 0;
}