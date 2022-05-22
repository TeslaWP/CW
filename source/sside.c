#include "sside.h"
#include "lib/linkedlist.h"
pthread_mutex_t mutex;
typedef struct clients{
    block key;
    int cfd;
} client;
SL_list * allclients;
int fd;

SL_list * SL_search_client(SL_list*L, int key){
    SL_list* r = L;
	do {
        r = r->next;
		if (((client*)(r->data))->cfd == key) return r;
	} while(r->next);
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
    SL_list* nc = allclients;
    if (nc->next == 0) {
        return;
    }
    pthread_mutex_lock(&mutex);
    do {
        nc = nc->next;
        client* allclientsi = (client*)(nc->data);
        if (allclientsi->cfd != curr) {
            block* en_send_msg = EncryptString(msg, allclientsi->key);
            int en_length = GetDataSize(en_send_msg);
            if (send(allclientsi->cfd, en_send_msg, en_length, 0) < 0){
                printf("Sending failure\n");
            }
            free(en_send_msg);
        }
    } while (nc->next);
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
        free(deenmsg);
    }
    pthread_mutex_lock(&mutex);
    SL_remove(allclients,SL_search_client(allclients, the.cfd));
    pthread_mutex_unlock(&mutex);
    return null;
}

void cleanclients() {
    pthread_mutex_lock(&mutex);
    SL_list* nc;
    SL_list* L = allclients;
    while (L != null){
		nc = L->next;
		free(nc->data);
		L = nc;
	}
    SL_destroy(allclients);
    pthread_mutex_unlock(&mutex);
}

void closeeverything() {
    printf("Завершаем процесс.\n");
    SL_list* nc = allclients;
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
    allclients = SL_create(0);
    while((cfd = accept(fd, (struct sockaddr *)NULL, NULL))) {
        block fKey = handshake(&cfd);
        //printf("fKey = %llx\n", fKey);
        sendtoall("Кто-то подключился! \n", cfd);
        pthread_mutex_lock(&mutex);
        client *newclient = malloc(sizeof(client));
        newclient->key = fKey;
        newclient->cfd = cfd;
        SL_add_first(allclients,SL_create(newclient));
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
