#include <stdio.h>
#include <malloc.h>
#include "phonebookStruct.h"

/*Функция добавления нового элемента в справочник. 
На вход получает указатель на структуру и указатель на размер справочника.
В случае, если в справочнике уже есть элемент(ы) выделенная для справочника память
увеличивается на размер одного элемента и элемент записывается в конец справочника.
Размер справочника увеливается на единицу и возвращается указатель на
изменённый блок памяти (блок может быть перемещён на новое место). */
struct Phonebook *addNewElem(struct Phonebook *newPhonebook, int *sizeOfPhonebook){
	if(*sizeOfPhonebook != 0){
		newPhonebook = (struct Phonebook *)realloc(newPhonebook, (sizeof(struct Phonebook))*(*sizeOfPhonebook+1));
	}

	printf("Введите имя (менее 10 символов ): \n");
	scanf("%7s", &(newPhonebook[*sizeOfPhonebook].name));
	printf("Введите фамилию (менее 10 символов): \n");
	scanf("%11s", &(newPhonebook[*sizeOfPhonebook].surname));	
	printf("Введите номер телефона (менее 12 символов): \n");
	scanf("%11s", &(newPhonebook[*sizeOfPhonebook].telephoneNumber));

	(*sizeOfPhonebook)++;

	return newPhonebook;
}

/*Функция вывода элементов справочника на экран в порядке их добавления.
На вход получает указатель на структуру и размер справочника.
Если в справочнике нет элементов, выводится сообщение об ошибке.*/
void printPhonebook(struct Phonebook *newPhonebook, int sizeOfPhonebook){
	if(sizeOfPhonebook == 0){
		printf("Справочник пуст\n");
	}
	else{
		for (int i = 0; i < sizeOfPhonebook; i++){
			printf("Имя:  %s\n", newPhonebook->name );
			printf("Фамилия:  %s\n", newPhonebook->surname );
			printf("Номер телефона:  %s\n", newPhonebook->telephoneNumber );
			newPhonebook++;
		}
	}
}

/*Функция поиска элемента в справочнике по номеру телефона.
На вход получает указатель на структуру и размер справочника.
Запрашивает номер телефона, по которому будет осуществляться поиск.
Сравнивает введённый с клавиатуры номер с номерами существующих абонентов.
Сравнение осуществляется по массивам типа char посимвольно в цикле.
Если в справочнике нет элемента с введённым номером телефона, выводится
сообщение об ошибке. Функция возвращает номер элемента в списке, начиная 
с единицы, или 0 в случае, если элемент не найден.*/
int searchByTelephoneNumber(struct Phonebook *newPhonebook,int sizeOfPhonebook){
	int flag = 1;//показатель существования элемента с номером, по которому осуществляется поиск
	int numOfSearch = 0;//номер найденного элемента
	int j;//счётчик по массиву типа char, хранящему номер телефона
	char searchPhoneNumber[sizeof(newPhonebook->telephoneNumber)];//номер, по которому осуществляется поиск

	printf("Введите номер телефона, по которому будет осуществляться поиск.\n");
	scanf("%11s", &searchPhoneNumber);

	for (int i = 0; i < sizeOfPhonebook; i++){
		j = 0;

		while((newPhonebook->telephoneNumber[j] != '\0') || (searchPhoneNumber[j] != '\0')){
			if(newPhonebook->telephoneNumber[j] != searchPhoneNumber[j]){
				flag = 0;
				break;
			}
			j++;
		}

		if(flag == 1){
			printf("Имя:  %s\n", newPhonebook->name );
			printf("Фамилия:  %s\n", newPhonebook->surname );
			printf("Номер телефона:  %s\n", newPhonebook->telephoneNumber );
			numOfSearch = i+1;
		}

		flag = 1;	
		newPhonebook++;
	}

	if(numOfSearch == 0){
		printf("Абонент с таким номером не найден!\n");
	}

	return numOfSearch;
}

/*Функция удаления элемента из справочника.
На вход получает указатель на структуру и указатель на размер справочника.
В функции осуществляется вызов функции searchByTelephoneNumber для поиска 
элемента, подлежащего удалению. Все элементы справочника после удаляемого 
сдвигаются на один(элемент) влево. Далее функцией realloc справочник урезается 
на один элемент с конца. Размер справочника уменьшается на один.
Возвращается указатель на структуру.*/
struct Phonebook *deleteElem(struct Phonebook *newPhonebook, int *sizeOfPhonebook){
	int elemToDelete = searchByTelephoneNumber(newPhonebook, *sizeOfPhonebook);

	if (elemToDelete != 0){
		for(int i = elemToDelete-1; i < *sizeOfPhonebook-1; i++){
			*(newPhonebook[i].name) = *(newPhonebook[i+1].name);
			*(newPhonebook[i].surname) = *(newPhonebook[i+1].surname);
			*(newPhonebook[i].telephoneNumber) = *(newPhonebook[i+1].telephoneNumber);
		}	
		(*sizeOfPhonebook)--;
		if(*sizeOfPhonebook != 0){
			newPhonebook = (struct Phonebook *)realloc(newPhonebook, (sizeof(struct Phonebook))*(*sizeOfPhonebook));	
		}
	}

	return newPhonebook;
}

