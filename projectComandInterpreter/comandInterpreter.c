#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/*****************************************************************************
**Программа является упрощённым командным интерпретатором, в котором        **
**используется канал. Стандартный поток вывода команды 1 подключается       **
**к стандартному потоку ввода команды 2. Для запуска программы пользователю **
**необходимо ввести название исполняемого файла, затем через пробелы ввести **
**команды и их ключи по следующему образцу: a.out com1 key1 com2 key2.      **                                                         **
*****************************************************************************/

#define SIZE_OF_COM 30
int fd[2]; //0 - read, 1 - write

void leftCom(char * argv[]){
    close(fd[0]); 
    dup2(fd[1], 1);
    execl(argv[1], argv[2], argv[3], NULL); //argv[1] - это полный путь, argv[2] - команда, argv[3] - ключ
}

void rightCom(char * argv[]){
    close(fd[1]); 
    dup2(fd[0], 0);
    execl(argv[4], argv[5], argv[6], NULL); //argv[4] - это полный путь, argv[5] - команда, argv[6] - ключ
}

void main(int argc, char * argv[]){
	pid_t pid, done;
	int *ptr_fd = fd;
	int status;
    size_t result;

    
    if(pipe(ptr_fd) != 0){
        perror("Error in creating the pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    
    //если процесс дочерний выполняем первую команду
    if(pid == 0){
        printf("In child process,  pid = %d,  ppid = %d\n", getpid(),  getppid());  
        leftCom(argv);
    } 
    else if(pid == -1){
	    perror("Error in creating the process\n");
	    exit(EXIT_FAILURE);
	}
	//если процесс родительский выполняем вторую команду
    else{
	    printf("In parent,  pid = %d,  fork returned = %d\n", getpid(),  pid);
	    rightCom(argv);
        done = wait(&status);
        if((status == -1) && (errno != ECHILD)){
            perror("Error during wait()");
            exit(EXIT_FAILURE);
        }
    }

    close(fd[0]);
    close(fd[1]);
    exit(EXIT_SUCCESS);
}