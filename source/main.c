#include "main.h"

int StartProcess(char filename[], char* argv[]) {
    pid_t pid=fork();
    if (pid==0) { 
        if (execv(filename, argv) == -1) {
            perror("StartProcess");
        }
        exit(127);
    }
    else {
        waitpid(pid,0,0);
    }
    return 0;
}

int main(){
    printf("=== Многопользовательский чат с шифрованием ===\n\n");
    printf("1: Подключиться\n");
    printf("2: Создать сервер\n\n");

    char* PassArgs1[] = {"server", NULL};
    char* PassArgs2[] = {"client", NULL};

    char Option;
    printf("> ");
    scanf("%c",&Option);
    if (Option == '2') {
        StartProcess("server",PassArgs1);
    } else {
        StartProcess("client",PassArgs2);
    }
    return 0;
}
