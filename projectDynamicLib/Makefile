.PHONY: all clean

all: libintNumCalcDyn.so resultDyn 

clean:
	rm -rf resultDyn libintNumCalcDyn.so

libintNumCalcDyn.so: intNumCalcFunc.c
	gcc -shared intNumCalcFunc.c -o libintNumCalcDyn.so 

resultDyn: test.c
	gcc test.c -L. -lintNumCalcDyn -o resultDyn
	export LD_LIBRARY_PATH=.; ./resultDyn





