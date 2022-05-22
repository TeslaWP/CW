#include "sside.h"
#include "lib/encrypt.h"
pthread_mutex_t mutex;
typedef struct clients{
    block key;
    int cfd;
} client;
client* allclients[20];
int fd;
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

void *sendkey(void *infd, block publicKey){
    int sock = *((int*)infd);
    char pK[sizeof(block)];
    memcpy(pK, &publicKey, sizeof(block));
    int wrt;
    if((wrt = write(sock, pK, strlen(pK)))==-1){
        perror("sendkey write() error: ");
    }
    return NULL;
}

block handshake(void *infd){
    block privateKey = GeneratePrivateKey();
    block publicKey = CalculatePublicKey(privateKey);
    block rcvdKey = *receivekey(infd);
    sendkey(infd, publicKey);
    block finalKey = CalculateFinalKey(privateKey, rcvdKey);
    //printf("my privateKey = %llx\n my publicKey = %llx\n their publicKey = %llx\n", privateKey, publicKey, rcvdKey);
    return finalKey;
}

void sendtoall(char *msg, int curr){
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < n; i++) {
        if (allclients[i]->cfd != curr) {
            block* en_send_msg = EncryptString(msg, allclients[i]->key);
            int en_length = GetDataSize(en_send_msg);
            if (send(allclients[i]->cfd, en_send_msg, en_length, 0) < 0){
                printf("Sending failure\n");
                continue;
            }
        }
    }
    printf("%s", msg);
    pthread_mutex_unlock(&mutex);
}

void *receivemsg(void *inc){
    client the = *((client *)inc);
    int sock = the.cfd;
    char msg[256];
    char *deenmsg;
    int len;
    while((len = recv(sock, msg, 256, 0)) > 0) {
        deenmsg = DecryptString((block*)msg, the.key);
        deenmsg[256] = '\0';
        sendtoall(deenmsg, sock);
    }
    return NULL;
}

void closeeverything() {
    for(int i = 0; i < 20; i++){
        close(allclients[i]->cfd);
    }
    close(fd);
}

int main(int argc, char *argv[]){
    struct sockaddr_in serv;
    int cfd, inport;
    pthread_t recvt;
    
    printf("Введите свой порт:\n>");
    scanf("%d",&inport);

    serv.sin_family = AF_INET;
    serv.sin_port = htons(inport);
    serv.sin_addr.s_addr = INADDR_ANY;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
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
    atexit(closeeverything);
    while((cfd = accept(fd, (struct sockaddr *)NULL, NULL))) {
        block fKey = handshake(&cfd);
        //printf("fKey = %llx\n", fKey);
        sendtoall("Кто-то подключился! \n", fd);
        pthread_mutex_lock(&mutex);
        client *newclient = malloc(sizeof(client));
        newclient->key = fKey;
        newclient->cfd = cfd;
        //printf("newclient->fKey = %llx\n", newclient->key);
        //printf("newclient->cfd = %d\n", newclient->cfd);
        allclients[n] = newclient;
        n++;
        
        pthread_create(
            &recvt,
            NULL,
            (void *)receivemsg, 
            newclient
        );
        
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}
