.PHONY: all

all: server.o serverFunc.o server

server.o: server.c 
	gcc -c server.c

serverFunc.o: serverFunc.c
	gcc -c serverFunc.c

server: server.o
	gcc server.o serverFunc.o -o server -lpthread
	./server