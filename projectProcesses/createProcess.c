#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


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
 
    pid_t pid[5];//pidFirst, pidSecond, pidThird, pidFourth, pidFifth;
 
    printf("Original program,  pid=%d\n",  getpid());

    //Создаются первые два процесса: родительский и дочерний
    pid[0] = fork();
    if(pid[0] == 0){
        printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid());
        // Если процесс дочерний, то для него создаётсЯ ещё один дочерний процесс  
        pid[1] = fork();
        processStat(pid[1]);
    } 
    else if(pid[0] == -1){
        printf("Error!\n");
        exit(1);
    }
    else{
        printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pid[0]);
        // Если процесс родительский, то для него создаётся ещё один дочерний процесс
        pid[2] = fork();

        if(pid[2] == 0){
            printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid()); 
            // Для дочернего процесса создаётся свой первый дочерний процесс 
            pid[3] = fork();

            if(pid[3] == 0){
            printf("In child process,  pid=%d,  ppid=%d\n", getpid(),  getppid());  
            } 
            else if(pid[3] == -1){
	            printf("Error!\n");
	            exit(1);
	        }
	        else{
	            printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pid[3]);
	            // Для дочернего процесса создаётся свой второй дочерний процесс
	            pid[4] = fork();
	            processStat(pid[4]);
		    }
        }
        else if(pid[2] == -1){
            printf("Error!\n");
            exit(1);
        }
        else{
            printf("In parent,  pid=%d,  fork returned=%d\n", getpid(),  pid[2]);
        }

        for(int i = 0; i < 5; i++){
            wait(pid[i]);
        }
        
    }
    
    exit(EXIT_SUCCESS);
}