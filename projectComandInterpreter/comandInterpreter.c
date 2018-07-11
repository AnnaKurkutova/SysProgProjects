#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/********************************************************************************
**Программа является упрощённым командным интерпретатором, в котором           **   
**используется канал. Стандартный поток вывода команды 1 подключается к        **
**стандартному потоку ввода команды 2 и т.д. Для запуска программы пользователю**
**необходимо ввести название исполняемого файла, затем через пробелы ввести    ** 
**путь до команды, команду, её ключ по следующему образцу:                     **      
**a.out path1 com1 key1 path2 com2 key2 path3 com3 key3 path4 com4 key4.       **
**Команды выполняются парами com1|com2, com3|com4 и т.д. Ограничение программы **
**состоит в том, что для одной команды можно ввести лишь один ключ, так как    **
**аргументы задаются пользователем при запуске программы.                      **
********************************************************************************/

#define SIZE_OF_COM 30
int fd[2]; //0 - read, 1 - write

void leftCom(char * argv[], int i){
    close(fd[0]); 
    dup2(fd[1], 1);
    execl(argv[i], argv[i+1], argv[i+2], NULL); //argv[i] - это полный путь, argv[i+1] - команда, argv[i+2] - ключ
    close(fd[1]);
}

void rightCom(char * argv[], int i){
    close(fd[1]); 
    dup2(fd[0], 0);
    execl(argv[i+3], argv[i+4], argv[i+5], NULL); //argv[i+3] - это полный путь, argv[i+4] - команда, argv[i+5] - ключ 
    close(fd[0]);
}

void main(int argc, char * argv[]){
    pid_t pid_first, pid_second, done;
    int *ptr_fd = fd;
    int status;
    const int pid_num = 2; //количество вызовов fork() для создания дочерних процессов
    size_t result;
    
    for(int i = 1; i < argc; i += 6){
        if(pipe(ptr_fd) != 0){
            perror("Error in creating the pipe");
            exit(EXIT_FAILURE);
        }
        pid_first = fork();
        //если процесс дочерний, выполняем первую команду
        if(pid_first == 0){
            pid_second = fork(); 
            //если процесс дочерний, выполняем вторую команду
            if(pid_second == 0){
                leftCom(argv, i);
            }
            else if(pid_second == -1){
                perror("Error in creating the process\n");
                exit(EXIT_FAILURE);
            }
            else{
                rightCom(argv, i);
            }
        } 
        else if(pid_first == -1){
            perror("Error in creating the process\n");
            exit(EXIT_FAILURE);
        }
    }
    //ожидание завершения дочерних процессов
    for(int i = 0; i < pid_num; i++){
        done = wait(&status);
        if((status == -1) && (errno != ECHILD)){
            perror("Error during wait()");
            exit(EXIT_FAILURE);
        }
    }  
    exit(EXIT_SUCCESS);
}