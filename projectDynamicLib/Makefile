.PHONY: all

all: resultDyn test.o intNumCalcFunc.o libintNumCalcDyn.so

resultDyn: test.o
	gcc test.o -L. -lintNumCalcDyn resultDyn
	export LD_LIBRARY_PATH=.; ./resultDyn

test.o: test.c
	gcc -c -o test.o test.c

intNumCalcFunc.o: intNumCalcFunc.c
	gcc -c -o intNumCalcFunc.o intNumCalcFunc.c

libintNumCalcDyn.so: intNumCalcFunc.o
	gcc -shared intNumCalcFunc.o libintNumCalcDyn.so

