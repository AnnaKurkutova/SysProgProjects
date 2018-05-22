#include <stdio.h>
#include "complexNumCalculator.h"

/*Операция сложения двух комплексных чисел. 
Числа: z1 = a+i*b; z2=c+i*d.
Сумма: z = (a+c)+i*(b+d).
*/
struct ComplexNum  add(struct ComplexNum firstValue, struct ComplexNum secondValue){
	struct ComplexNum resultValue;

	resultValue.realPart = firstValue.realPart + secondValue.realPart;
	resultValue.imaginaryPart = firstValue.imaginaryPart + secondValue.imaginaryPart;

	return resultValue;
}

/*Операция вычитания одного комплексного числа из другого.
Числа: z1 = a+i*b; z2=c+i*d.
Разность: z = (a-c)+i*(b-d).
*/
struct ComplexNum subtract(struct ComplexNum firstValue, struct ComplexNum secondValue){
	struct ComplexNum resultValue;

	resultValue.realPart = firstValue.realPart - secondValue.realPart;
	resultValue.imaginaryPart = firstValue.imaginaryPart - secondValue.imaginaryPart;

	return resultValue;
}

/*Операция умножения двух комплексных чисел. 
Числа: z1 = a+i*b; z2=c+i*d.
Произведение: z = (a*c-b*d)+i*(b*c+a*d).
*/
struct ComplexNum multiply(struct ComplexNum firstValue, struct ComplexNum secondValue){
	struct ComplexNum resultValue;

	resultValue.realPart = firstValue.realPart * secondValue.realPart - firstValue.imaginaryPart * secondValue.imaginaryPart;
	resultValue.imaginaryPart = firstValue.imaginaryPart * secondValue.realPart + secondValue.imaginaryPart * firstValue.realPart;

	return resultValue;
}

/*Операция деления одного комплексного числа на другое.
Числа: z1 = a+i*b; z2=c+i*d.
Частное: z = (a*c+b*d)/(c^2+d^2)+i*(b*c-a*d)/(c^2+d^2).
*/
struct ComplexNum divide(struct ComplexNum firstValue, struct ComplexNum secondValue){
	struct ComplexNum resultValue;

	resultValue.realPart = (firstValue.realPart * secondValue.realPart + firstValue.imaginaryPart * secondValue.imaginaryPart) / (secondValue.realPart * secondValue.realPart + secondValue.imaginaryPart * secondValue.imaginaryPart);
	resultValue.imaginaryPart = (firstValue.imaginaryPart * secondValue.realPart - firstValue.realPart * secondValue.imaginaryPart) / (secondValue.realPart * secondValue.realPart + secondValue.imaginaryPart * secondValue.imaginaryPart);

	return resultValue;
}

/*Вывод комплексного числа на экран в алгебраической форме.*/
void printComplex(struct ComplexNum* value){
	printf("Результат = %f + i*%f \n", value->realPart, value->imaginaryPart);
}

/*Ввод комплексного числа с клавиатуры. 
Сначала вводится реальная часть, затем мнимая.*/
void scanComplex(struct ComplexNum* value){
	printf("Реальная часть = \n");
	scanf("%f", &(value->realPart));
	printf("Мнимая часть = \n");
	scanf("%f", &(value->imaginaryPart));
}