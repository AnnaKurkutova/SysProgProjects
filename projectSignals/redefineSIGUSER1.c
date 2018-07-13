#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/******************************************
**Программа обрабатывает сигнал SIGUSR1: **
**при получении этого сигнала программа  **
**выводит диагностическое сообщение и    **
**успешно завершает свою работу.         **
******************************************/

/* Обработчик сигнала */
void my_handler(int signum)
{
    if (signum == SIGUSR1){
        printf("Received SIGUSR1!\n");
    }
    exit(EXIT_SUCCESS);
}

void main(){
    int value = 0;
    struct sigaction sa;

    /* выводим pid процесса, чтобы в программе подачи 
    сигнала SIGUSR1 поместить его в параметры kill(pid, SIGUSR1)*/
    printf("Original program,  pid=%d\n",  getpid());
    /* устанавливаем обработчик сигнала */ 
    sa.sa_handler = my_handler;
    sigaction(SIGUSR1, &sa, 0);

    while(1){
        value++;
        printf("%d\n", value);
        sleep(2);
    }
}
