.PHONY: all

all: intNumCalcFunc.o libintNumCalcDyn.so test.o resultDyn 

intNumCalcFunc.o: intNumCalcFunc.c
	gcc -fPIC -c intNumCalcFunc.c

libintNumCalcDyn.so: intNumCalcFunc.o
	gcc -shared -o libintNumCalcDyn.so intNumCalcFunc.o

test.o: test.c
	gcc -c test.c

resultDyn: test.o
	gcc test.o -L. -lintNumCalcDyn -o resultDyn
	export LD_LIBRARY_PATH=.; ./resultDyn





