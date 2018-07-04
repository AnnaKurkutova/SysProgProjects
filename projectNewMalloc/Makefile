.PHONY: all

all: myMalloc.o libmyMalloc.so test.o test 

myMalloc.o: myMalloc.c
	gcc -fPIC -c myMalloc.c

libmyMalloc.so: myMalloc.o
	gcc -shared -o libmyMalloc.so myMalloc.o

test.o: test.c
	gcc -c test.c

test: test.o
	gcc test.o -L. -lmyMalloc -o test
	export LD_LIBRARY_PATH=.; ./test
