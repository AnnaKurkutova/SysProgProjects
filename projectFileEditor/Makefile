.PHONY: all

all: fileEditor.o fileEditorFunc.o test

fileEditor.o: fileEditor.c
	gcc -c fileEditor.c

fileEditorFunc.o: fileEditorFunc.c
	gcc -c fileEditorFunc.c

#Компилляция и запуск тестовой программы.
test: fileEditor.o fileEditorFunc.o
	gcc ./fileEditor.o ./fileEditorFunc.o -o test -lncurses
	./test
