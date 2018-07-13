#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/***********************************************
**Программа блокирует сигнал SIGUSR1, так что **
**этот сигнал не может завершить ее работу.   **
***********************************************/

void main(){
    int value = 0;

    /* выводим pid процесса, чтобы в программе подачи 
    сигнала SIGUSR1 поместить его в параметры kill(pid, SIGUSR1) */
    printf("Original program,  pid=%d\n",  getpid()); 
    
    /* блокируем сигнал  SIGUSR1 */
    sigset_t newset;
    sigemptyset(&newset);
    sigaddset(&newset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &newset, 0);
    while(1){
        value++;
        printf("%d\n", value);
        pause(); //программа ждёт сигнал, но не дождётся, потому что он заблокирован
    }
}