.PHONY: all

all: fileMenu.o fileMenuFunc.o fileMenu

fileMenu.o: fileMenu.c
	gcc -c fileMenu.c

fileMenuFunc.o: fileMenuFunc.c
	gcc -c fileMenuFunc.c

#Компилляция и запуск тестовой программы.
fileMenu: fileMenu.o fileMenuFunc.o
	gcc ./fileMenu.o ./fileMenuFunc.o -o fileMenu -lpanel -lncurses
	valgrind --leak-check=full --show-leak-kinds=all ./fileMenu
