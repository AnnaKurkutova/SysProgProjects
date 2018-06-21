.PHONY: all

all: intNumCalcFunc.o libintNumCalc.a test.o result

intNumCalcFunc.o: intNumCalcFunc.c
	gcc -c intNumCalcFunc.c

libintNumCalc.a: intNumCalcFunc.o
	ar rc libintNumCalc.a intNumCalcFunc.o

test.o: test.c
	gcc -c test.c

result: test.o
	gcc test.o -L. -lintNumCalc -o result
	./result





