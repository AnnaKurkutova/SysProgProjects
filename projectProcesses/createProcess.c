#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


/*******************************************
********************************************
**     Перед первым pidFirst = fork();    **
**                P                       **
**                                        **
**     После первого pidFirst = fork()    **
**                P                       **
**              /                         **
**           P(1)                         **
**                                        **
**     После второго pidSecond = fork()   **
**                P                       **
**              /                         **
**           P(1)                         **
**           /                            **
**     P(1)(2)                            **
**                                        **
**     После третьего pidThird = fork()   **
**                P                       **
**              /  \                      **
**           P(1)  P(2)                   **
**           /                            **
**     P(1)(2)                            **
**                                        **
**     После четвёртого pidFourth = fork()**
**                P                       **
**              /  \                      **
**           P(1)  P(2)                   **
**           /     /                      **
**     P(1)(2)  P(2)(3)                   **
**                                        **
**     После пятого pidFifth = fork()     **
**                P                       **
**              /  \                      **
**           P(1)  P(2)                   **
**           /     /   \                  **
**     P(1)(2) P(2)(3) P(2)(4)            **
**                                        **
********************************************
*******************************************/ 

// Вывод свойств процесса
void processStat(pid_t pid){
    if(pid == 0){
        printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid());  
    } 
    else if(pid == -1){
        printf("Error!\n");
        exit(1);
    }
    else{
        printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pid);
    }
}

int main (void) {
 
    pid_t pidFirst, pidSecond, pidThird, pidFourth, pidFifth;
 
    printf("Original program,  pid=%d\n",  getpid());

    //Создаются первые два процесса: родительский и дочерний
    pidFirst = fork();
    if(pidFirst == 0){
        printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid());
        // Если процесс дочерний, то для него создаётсЯ ещё один дочерний процесс  
        pidSecond = fork();
        processStat(pidSecond);
    } 
    else if(pidFirst == -1){
        printf("Error!\n");
        exit(1);
    }
    else{
        printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pidFirst);
        // Если процесс родительский, то для него создаётся ещё один дочерний процесс
        pidThird = fork();

        if(pidThird == 0){
            printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid()); 
            // Для дочернего процесса создаётся свой первый дочерний процесс 
            pidFourth = fork();

            if(pidFourth == 0){
            printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid());  
            } 
            else if(pidFourth == -1){
	            printf("Error!\n");
	            exit(1);
	        }
	        else{
	            printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pidFourth);
	            // Для дочернего процесса создаётся свой второй дочерний процесс
	            pidFifth = fork();
	            processStat(pidFifth);

		    }

        }
        else if(pidThird == -1){
            printf("Error!\n");
            exit(1);
        }
        else{
            printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pidThird);
        }


    }
    
    exit(EXIT_SUCCESS);
}