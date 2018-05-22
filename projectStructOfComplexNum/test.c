/*Данная программа является калькулятором комплексных чисел, 
в котором доступны операции сложения, вычитания, умножения и деления.*/

#include <stdio.h>
#include "complexNumCalculator.h"

int main (void){
	struct ComplexNum firstValue;//первое число
	struct ComplexNum secondValue;//второе число
	struct ComplexNum resultValue;//результирующее значение
	int num;//номер операции
	int flag = 1;//показатель продолжения(flag == 1)/завершения(flag == 0) цикла

	printf("Введите первое число\n");
	scanComplex(&firstValue);
	printf("Введите второе число\n");
	scanComplex(&secondValue);

	//меню выбора операций над комплексными числами
    do{
    printf("Выберете операцию из списка:\n1.Сложение\n2.Вычитание\n3.Умножение\n4.Деление\n");
    scanf("%d", &num);

    switch(num){
    	case 1: {resultValue = add(firstValue, secondValue);
			      printComplex(&resultValue);
			      }
			      break;

    	case 2: {resultValue = subtract(firstValue, secondValue);
			      printComplex(&resultValue);
			  	  }
			      break;

    	case 3: {resultValue = multiply(firstValue, secondValue);
		    	  printComplex(&resultValue);
		    	  }
		    	  break;

    	case 4: {resultValue = divide(firstValue, secondValue);
		    	  printComplex(&resultValue);
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

