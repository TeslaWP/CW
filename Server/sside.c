#include "header.h"
struct sockaddr_in serv; //Главная переменная сокетоа
int fd; //Файловый дескриптор сокета, используемый для его идентификации
int conn; //Файловый дескриптор соединения, используемый для отличия подключений клиента
char message[100] = ""; //Массив, содержащий сообщения.

int main(){
    serv.sin_family = AF_INET;
    serv.sin_port = htons(1337); //Define the port at which the server will listen for connections.
    printf("%hu\n",ntohs(serv.sin_port));
    serv.sin_addr.s_addr = INADDR_ANY;
    fd = socket(AF_INET, SOCK_STREAM, 0); //This will create a new socket and also return the identifier of the socket into fd.
    // To handle errors, you can add an if condition that checks whether fd is greater than 0. If it isn't, prompt an error
    if (bind(fd, (struct sockaddr *)&serv, sizeof(serv))) {
	perror("bind");
    }; //assigns the address specified by serv to the socket
    listen(fd,5); //Listen for client connections. Maximum 5 connections will be permitted.
    //Now we start handling the connections.
    while((conn = accept(fd, (struct sockaddr *)NULL, NULL))) {
        int pid;
        if((pid = fork()) == 0) {
            while (recv(conn, message, 100, 0)>0) {
                printf("Message Received: %s\n", message);
                //An extra breaking condition can be added here (to terminate the child process)            
                strcpy(message, "");
            }
            exit(0);
        }
    }
    exit(0);
}
