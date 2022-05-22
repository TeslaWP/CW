#include "sside.h"
#include "lib/deque.h"

pthread_mutex_t mutex;
typedef struct clients{
    block key;
    int cfd;
} client;

deque* allclients;
int fd;

deque * DQ_search_client(deque*L, int key){
    deque* r = L;
    r = r->next;
	while(r!=L) {
        r = r->next;
		if (((client*)(r->data))->cfd == key) return r;
	}
	return null;
}

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
    return null;
}

block handshake(void *infd){
    block privateKey = GeneratePrivateKey();
    block publicKey = CalculatePublicKey(privateKey);
    block *rcvdKey = receivekey(infd);
    sendkey(infd, publicKey);
    block finalKey = CalculateFinalKey(privateKey, *rcvdKey);
    free(rcvdKey);
    //printf("my privateKey = %llx\n my publicKey = %llx\n their publicKey = %llx\n", privateKey, publicKey, rcvdKey);
    return finalKey;
}

void sendtoall(char *msg, int curr){
    pthread_mutex_lock(&mutex);
    deque* nc = allclients;
    if (nc->next == nc) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    nc = nc->next;
    while (nc!=allclients) {
        client* allclientsi = (client*)(nc->data);
        if (allclientsi->cfd != curr) {
            block* en_send_msg = EncryptString(msg, allclientsi->key);
            int en_length = GetDataSize(en_send_msg);
            if (send(allclientsi->cfd, en_send_msg, en_length, 0) < 0){
                printf("Sending failure\n");
            }
            free(en_send_msg);
        }
        nc = nc->next;
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
        strcat(deenmsg, "\n");
        sendtoall(deenmsg, sock);
        free(deenmsg);
    }
    pthread_mutex_lock(&mutex);
    DQ_delete(allclients, DQ_search_client(allclients, the.cfd), (void*)0);
    pthread_mutex_unlock(&mutex);
    return null;
}

void cleanclients() {
    pthread_mutex_lock(&mutex);
    DQ_destroy(allclients);
    pthread_mutex_unlock(&mutex);
}

void closeeverything() {
    printf("Завершаем процесс.\n");
    deque* nc = allclients;
    do {
        nc=nc->next;
        close(((client*)(nc->data))->cfd);
    } while (nc->next != null);
    close(fd);
    cleanclients();
}

int main(int argc, char *argv[]){
    struct sockaddr_in serv;
    int cfd, inport;
    pthread_t recvt;
    
    printf("Введите свой порт:\n>");
    scanf("%d",&inport);
    if (inport==0) inport = 6969;

    serv.sin_family = AF_INET;
    serv.sin_port = htons(inport);
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
    atexit(closeeverything);
    allclients = DQ_create();
    while((cfd = accept(fd, (struct sockaddr *)NULL, NULL))) {
        block fKey = handshake(&cfd);
        //printf("fKey = %llx\n", fKey);
        pthread_mutex_lock(&mutex);
        client * newclient = malloc(sizeof(client));
        newclient->key = fKey;
        newclient->cfd = cfd;
        DQ_lpush(allclients,newclient,sizeof(client));
        pthread_create(
            &recvt,
            null,
            (void *)receivemsg, 
            newclient
        );
        pthread_mutex_unlock(&mutex);
        sendtoall("Кто-то подключился! \n", cfd);
    }
    return 0;
}
