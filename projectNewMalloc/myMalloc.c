#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include "myMalloc.h"

/* Глобальные переменные */
int isInitialized = 0;
void *memoryStart;//
void *lastValidAddr;// Последний действительный адрес

/* Инициализация блока памяти */
void my_init(){
	lastValidAddr = sbrk(0);// Запрашивается последний действительный адрес
	memoryStart = lastValidAddr;// Установка начального указателя на последний действительный адрес
 	isInitialized = 1;// Инициализация прошла
 	printf(" Память была инициализирована\n");
}

void *my_malloc(size_t numBytes){
	void *curLocation;// Место откуда начинается поиск
	struct blockOfData *curLocationBlock;// Будет содержать данные о curLocation
	void *memLoc;// Будет указывать на найденную память. На время поиска он должен быть 0.

	// Инициализация, если она ещё не была сделана
	if(isInitialized != 1){
		my_init();
	}

	numBytes = numBytes + sizeof(struct blockOfData);//смещение указателя на размер структуры
	memLoc = 0;// Пока не найден подходящий участок памяти
	curLocation = memoryStart;// Текущий указатель устанавливается на начало доступной памяти

	// Поиск по доступному пространству
	while(curLocation != lastValidAddr){
		curLocationBlock = (struct blockOfData *)curLocation;
        
        //Выделение под новый блок данных памяти, которая уже использовалась ранее, но была высвобождена
		if((curLocationBlock->isFree == 1) && (curLocationBlock->size >= numBytes)){
			memLoc = curLocation;// Память найдена
			// Инициализация структуры
		    curLocationBlock = memLoc;
		    curLocationBlock->isFree = 0;// Память занята
		    curLocationBlock->size = numBytes;// Размер определён
		    printf("Была найдена память нужного размера в доступной области\n");
			break;
		}
		curLocation = curLocation + curLocationBlock->size;// Участок памяти не подошёл, указатель сдвигается
	}

    // Если память ещё не найдена, она запрашивается у ОС
	if(memLoc == 0){
		sbrk(numBytes);// Сдвиг границы в HEAP
		memLoc = lastValidAddr;// Память найдена, последний действительный адрес меняется
		lastValidAddr = lastValidAddr + numBytes;// Доступное пространство расширяется на numBytes

		// Инициализация структуры
		curLocationBlock = memLoc;
		curLocationBlock->isFree = 0;// Память занята
		curLocationBlock->size = numBytes;// Размер определён
		printf("Область памяти была расширена\n");
	}

	if(memLoc == 0){
		printf("Память не может быть выделена\n");
	}
	else{
        memLoc = memLoc + sizeof(struct blockOfData);// Указатель перемещается в конец структуры
	}
	return memLoc;
 }

/* Освобождение памяти */
void my_free(void *firstbyte){
	struct blockOfData *blockToFree;

	blockToFree = firstbyte - sizeof(struct blockOfData);// Перемещение на начало структуры
	blockToFree->isFree = 1;// Блок помечается как доступный
	printf("Память была высвобождена\n");
	return;
}

