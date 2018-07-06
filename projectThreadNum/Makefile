.PHONY: all

all: threadNum.o threadNumFunc.o test

threadNum.o: threadNum.c
	gcc -c threadNum.c

threadNumFunc.o: threadNumFunc.c
	gcc -c threadNumFunc.c

#Компилляция и запуск тестовой программы.
test: threadNum.o threadNumFunc.o
	gcc ./threadNum.o ./threadNumFunc.o -o test -lpthread
	./test