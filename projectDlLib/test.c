/*Данная программа является калькулятором целых чисел, 
в котором доступны операции сложения, вычитания, умножения и деления.
Для реализации динамической загрузки используется интерфейс 
динамической загрузки (Dynamic Loading API). 
Используемые функции интерфейса: dlopen, dlsym, dlclose. */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "intNumCalcFunc.h"

//Данная функция вызывает методы по адресу из их библиотек.
void funcFromLib(char *funcLib, char *func, int firstValue, int secondValue){
	void *dlHandle;//обработчик библиотеки
	int (*funcPtr)(int, int);//переменная для хранения адреса функции

	//загрузка библиотеки 
    dlHandle = dlopen(funcLib, RTLD_NOW);
    //поиск адреса функции в библиотеке
	funcPtr = dlsym(dlHandle, func);	
	//вызов функции по найденному адресу и вывод результата
  	printf("Результат равен %d\n", (*funcPtr)(firstValue, secondValue));
 	//отсоединение библиотеки от программы
 	dlclose(dlHandle);

}

int main (void){
	int firstValue;//первое число
	int secondValue;//второе число
	int num;//номер операции
	int flag = 1;//показатель продолжения(flag == 1)/завершения(flag == 0) цикла

	//меню выбора операций над целыми числами
    do{
    printf("Введите первое число\n");
	scanf("%d", &firstValue);
	printf("Введите второе число\n");
	scanf("%d", &secondValue);

    printf("Выберете операцию из списка:\n1.Сложение\n2.Вычитание\n3.Умножение\n4.Деление\n");
    scanf("%d", &num);

    switch(num){
    	case 1: { 
    		//операция поиска суммы целых чисел	
			funcFromLib("/home/slivka/gitProjects/projectDlLib/libaddIntegers.so", "addIntegers", firstValue, secondValue);
		}
		break;

    	case 2: {
    		//операция поиска разности целых чисел
    		funcFromLib("/home/slivka/gitProjects/projectDlLib/libsubtractIntegers.so", "subtractIntegers", firstValue, secondValue);
    	}
		break;

    	case 3: {
    		//операция поиска произведения целых чисел
    		funcFromLib("/home/slivka/gitProjects/projectDlLib/libmultiplyIntegers.so", "multiplyIntegers", firstValue, secondValue);
    	}
		break;

    	case 4: {
    		//операция поиска частного целых чисел
    		funcFromLib("/home/slivka/gitProjects/projectDlLib/libdivideIntegers.so", "divideIntegers", firstValue, secondValue);
    	}
		break;

	/*в случае ввода неверного номера операции происходит 
	выход из цикла без осуществления каких-либо вычислений*/
	default: printf("Нет операции с таким номером");
		break;
	}

	printf("Хотите продолжить вычисления?Да-1.Нет-0.");
	scanf("%d", &flag);

}while(flag == 1);

    getchar();//пауза
    return 0;

}