//Работа с указателями, разыменованием указателей и т.п.

#include <stdio.h>

int main(){
	//работа с одинарным указателем

	int a = 7;
	int *ptr;
	ptr = &a;
	*(ptr) += 6;
	printf("%d\n", *ptr);//вывод значения, на которое указывает ptr
	printf("%x\n", ptr);//вывод адреса в памяти, где хранится значение, на которое указывает ptr
	ptr += 4;
	printf("%x\n", ptr);//вывод адреса в памяти, где хранится значение, на которое указывает ptr
	ptr -= 4;
	printf("%x\n", ptr);//вывод адреса в памяти, где хранится значение, на которое указывает ptr
	printf("%x\n\n", &ptr);//вывод адреса в памяти, где хранится сам указатель

	//работа с двойным указателем

	int **ptr2;
	ptr2 = &ptr;
	a += 8;
	printf("%d\n", **ptr2);//вывод значения, на которое указывает ptr2
	*(ptr) += 1;
	printf("%d\n", **ptr2);//вывод значения, на которое указывает ptr2
	printf("%x\n", ptr2);//вывод адреса в памяти, где хранится ptr
	printf("%x\n", &ptr2);//вывод адреса в памяти, где хранится сам указатель ptr2
	printf("%x\n", *ptr2);//вывод адреса в памяти, где хранится значение, на которое указывает ptr2

}