.PHONY: all

all: myDaemon.o test

myDaemon.o: myDaemon.c
	gcc -c myDaemon.c

#Компилляция и запуск тестовой программы.
test: myDaemon.o
	gcc ./myDaemon.o -o test
	./test
	#Проверка работы программы
	ps -xj | grep myDaemon
	#Чтение syslog
	grep myDaemon /var/log/syslog
