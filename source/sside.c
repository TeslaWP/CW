#include "sside.h"
#include "lib/encrypt.h"
pthread_mutex_t mutex;
int clients[5];
block keys[5];
int n = 0;


block *receivekey(void *me){
    int sock = *((int*)me);
    char *keymsg;
    keymsg = malloc(sizeof(block));
    if(recv(sock, keymsg, sizeof(block), 0)==-1){
        perror("receivekey recv() error: ");
        exit(EXIT_FAILURE);
    }
    return (block *)keymsg;
}

void *sendkey(void *fd, block publicKey){
    int sock = *((int*)fd);
    char pK[sizeof(block)];
    memcpy(pK, &publicKey, sizeof(block));
    int wrt;
    if((wrt = write(sock, pK, strlen(pK)))==-1){
        perror("sendkey write() error: ");
    }
    return NULL;
}

block handshake(void *fd){
    block privateKey = GeneratePrivateKey();
    block publicKey = CalculatePublicKey(privateKey);
    block rcvdKey = *receivekey(fd);
    sendkey(fd, publicKey);
    block finalKey = CalculateFinalKey(privateKey, rcvdKey);
    return finalKey;
}

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

int main(int argc, char *argv[]){
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

    char *ip = inet_ntoa(serv.sin_addr);
    int port = htons(serv.sin_port);
    printf("Сервер запущен по адресу %s:%d\n",ip,port);
    
    while((cfd = accept(fd, (struct sockaddr *)NULL, NULL))) {
        block fKey = handshake(&cfd);
        printf("%llx\n", fKey);
        sendtoall("Кто-то подключился! \n", fd);
        pthread_mutex_lock(&mutex);
        clients[n] = cfd;
        keys[n] = fKey;
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
