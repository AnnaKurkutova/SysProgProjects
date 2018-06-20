/*Данная программа является калькулятором целых чисел, 
в котором доступны операции сложения, вычитания, умножения и деления.*/

#include <stdio.h>
#include "intNumCalcFunc.h"

int main (void){
	int firstValue;//первое число
	int secondValue;//второе число
	int resultValue;//результирующее значение
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
    	case 1: {resultValue = addIntegers(firstValue, secondValue);
		printf("Сумма введённых чисел равна %d\n", resultValue);
		}
		break;

    	case 2: {resultValue = subtractIntegers(firstValue, secondValue);
		printf("Разность введённых чисел равна %d\n", resultValue);
		}
		break;

    	case 3: {resultValue = multiplyIntegers(firstValue, secondValue);
		printf("Произведение введённых чисел равно %d\n", resultValue);
		}
		break;

    	case 4: {resultValue = divideIntegers(firstValue, secondValue);
		printf("Частное введённых чисел равно %d\n", resultValue);
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
