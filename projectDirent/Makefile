.PHONY: all

all: addIntegers.o libaddIntegers.so subtractIntegers.o libsubtractIntegers.so \
multiplyIntegers.o libmultiplyIntegers.so divideIntegers.o libdivideIntegers.so \
resultDl

#Компилляция файла с операцией сложения целых чисел и создание каталога plugin.
addIntegers.o: addIntegers.c
	gcc -fPIC -c addIntegers.c
	mkdir ./plugin

#Создание библиотеки для файла addIntegers.o.
libaddIntegers.so: addIntegers.o
	gcc -shared -o ./plugin/libaddIntegers.so addIntegers.o

#Компилляция файла с операцией вычитания одного целого числа из другого.
subtractIntegers.o: subtractIntegers.c
	gcc -fPIC -c subtractIntegers.c

#Создание библиотеки для файла subtractIntegers.o.
libsubtractIntegers.so: subtractIntegers.o
	gcc -shared -o ./plugin/libsubtractIntegers.so subtractIntegers.o

#Компилляция файла с операцией умножения целых чисел.
multiplyIntegers.o: multiplyIntegers.c
	gcc -fPIC -c multiplyIntegers.c

#Создание библиотеки для файла multiplyIntegers.o.
libmultiplyIntegers.so: multiplyIntegers.o
	gcc -shared -o ./plugin/libmultiplyIntegers.so multiplyIntegers.o

#Компилляция файла с операцией деления одного целого числа на другое.
divideIntegers.o: divideIntegers.c
	gcc -fPIC -c divideIntegers.c

#Создание библиотеки для файла divideIntegers.o.
libdivideIntegers.so: divideIntegers.o
	gcc -shared -o ./plugin/libdivideIntegers.so divideIntegers.o

#Компилляция и запуск тестовой программы.
resultDl: test.c
	gcc test.c -o resultDl -ldl
	./resultDl
