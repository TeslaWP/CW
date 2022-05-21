#include "header.h"
pthread_mutex_t mutex;
int clients[5];
int n = 0;

void sendtoall(char *msg, int curr){
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < n; i++) {
        if (clients[i] != curr) {
            if (send(clients[i], msg, strlen(msg), 0) < 0){
                printf("Sending failure\n");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *receivemsg(void *client_fd){
    int sock = *((int*)client_fd);
    char msg[256];
    int len;
    while((len = recv(sock, msg, 256, 0)) > 0) {
        msg[len] = '\0';
        sendtoall(msg, sock);
    }
    return NULL;
}

int main(){
    struct sockaddr_in serv;
    int fd, cfd;
    pthread_t recvt;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(1337);
    serv.sin_addr.s_addr = INADDR_ANY;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (bind(fd, (struct sockaddr *)&serv, sizeof(serv))) {
	    perror("bind() error: ");
        exit(EXIT_FAILURE);
    };
    
    if (listen(fd,5)==-1){
        perror("listen() error: ");
    }
    
    while((cfd = accept(fd, (struct sockaddr *)NULL, NULL))) {
        printf("%d", 1);
        pthread_mutex_lock(&mutex);
        clients[n] = cfd;
        n++;
        
        pthread_create(
            &recvt,
            NULL,
            (void *)receivemsg, 
            &cfd
        );

        pthread_mutex_unlock(&mutex);
    }
    return 0;
}
