/*Данная программа является реализацией работы с телефонным справочником.
Каждый элемент справочника включает имя, фамилию и номер телефона абонента.
Пользователь может добавлять новых абонентов, удалять существующих,осуществлять 
поиск абонента по номеру телефона и выводить список существующих в справочнике 
абонентов.*/

#include <stdio.h>
#include <malloc.h>//для работы с динамической памятью (функции malloc(), realloc(), free())
#include "phonebookStruct.h"

int main(void){ 
	int sizeOfPhonebook = 0;//размер телефонного справочника - количество абонентов
	int flag = 1;//показатель продолжения/завершения цикла do while
	int numOfChoice, numOfSearch;//номер операции из меню и номер найденного абонента в функции поиска по номеру
	struct Phonebook *newPhonebook;//указатель на структуру 
	newPhonebook = (struct Phonebook *)malloc(sizeof(struct Phonebook));//выделение динамической памяти под один элемент структуры

	//меню выбора операций над справочником
	do{
		printf("Выберете операцию из списка:\n1.Добавить в справочник новый элемент\n2.Удалить элемент из справочника\n3.Найти элемент по номеру телефона\n4.Вывести список элементов на экран\n5.Выйти\n");
	    scanf("%d", &numOfChoice);

		switch(numOfChoice){
			case 1 :{
				newPhonebook = addNewElem(newPhonebook, &sizeOfPhonebook);
			}
			break;
			case 2 :{
				newPhonebook = deleteElem(newPhonebook, &sizeOfPhonebook);				
			}
			break;
			case 3 :{
				numOfSearch = searchByTelephoneNumber(newPhonebook, sizeOfPhonebook);
			}
			break;	
			case 4 :{
				printPhonebook(newPhonebook, sizeOfPhonebook);
			}
			break;	
			case 5 :{
				flag = 0;
			}
			break;	
		}

	}while(flag == 1);

	free(newPhonebook);//высвобождение динамической памяти

	getchar();//пауза
	return 0;
}