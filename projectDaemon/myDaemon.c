#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

void myDaemon(){
    pid_t pid;
    int fileDescript;// для закрытия файловых дескрипторов

    pid = fork();

    // Дочерний процесс
    if(pid == 0){
        printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid());
        // Создаём сессию для дочернего процесса, он становится лидером сессии
        setsid();   
        // Закрываем таблицу дескрипторов
        // _SC_OPEN_MAX - максимальное количество файлов, которое может открыть процесс
        for (fileDescript = sysconf(_SC_OPEN_MAX); fileDescript >= 0; fileDescript--){
            close(fileDescript);
        }
        // Меняем рабочую директорию на /root
        chdir("/");
    } 
    // Ошибка
    else if(pid == -1){
        printf("Error!\n");
        exit(EXIT_FAILURE);
    }
    // Родительский процесс
    else{
        printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pid);
        // Завершаем процесс
        exit(EXIT_SUCCESS);
    }
    
    // LOG_PID указывает, что нужно добавлять к каждому сообщению идентификатор
    // LOG_DAEMON указывает, что тип программы, записывающей системные сообщения, - демон
    openlog ("myDaemon", LOG_PID, LOG_DAEMON);// установка связи с программой, ведущей системный журнал
}

void main(){
    myDaemon();

    while(1){
        // Степень важности сообщения - важные рабочие условия
        syslog (LOG_NOTICE, "was started");// Создание сообщения для журнала
        sleep (10);// Остановка
        break;
    }

    syslog (LOG_NOTICE, "was terminated");
    closelog();// Закрытие описателя, используемого для записи данных в журнал

    exit(EXIT_SUCCESS);
}