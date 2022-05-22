#include "clside.h"
#include "lib/encrypt.h"

char msg[256];
pthread_t recvt;
int fd;
block fKey;

block *receivekey(void *me){
    int sock = *((int*)me);
    char *keymsg;
    keymsg = malloc(sizeof(block));
    recv(sock, keymsg, sizeof(block), 0);
    return (block *)keymsg;
}

void *sendkey(void *infd, block publicKey){
    int sock = *((int*)infd);
    char pK[sizeof(block)];
    memcpy(pK, &publicKey, sizeof(block));
    write(sock, pK, strlen(pK));
    return NULL;
}

block handshake(void *infd){
    block privateKey = GeneratePrivateKey();
    block publicKey = CalculatePublicKey(privateKey);
    sendkey(infd, publicKey);
    block rcvdKey = *receivekey(infd);
    block finalKey = CalculateFinalKey(privateKey, rcvdKey);
    //printf("my privateKey = %llx\n my publicKey = %llx\n their publicKey = %llx\n", privateKey, publicKey, rcvdKey);
    return finalKey;
}

void *receivemsg(void *me){
    int sock = *((int*)me);
    int len;
    char* deenmsg;
    while((len = recv(sock, msg, 256, 0)) > 0) {
        deenmsg = DecryptString((block*)msg, fKey);
        msg[256] = '\0';
        fputs(deenmsg, stdout);
    }
    return NULL;
}

void closeeverything() {
    pthread_join(recvt, NULL);
    close(fd);
}

int main(int argc, char *argv[]){
    atexit(closeeverything);
    struct sockaddr_in serv;
    char send_msg[256];
    char client_name[32];
    char name[32];
    char ip[128];
    int port;

    printf("Адрес сервера:\n>");
    scanf("%31s",ip);

    printf("Порт:\n>");
    scanf("%d",&port);
        
    printf("Ваш логин:\n>");
    scanf("%127s",name);

    strcpy(client_name, name);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = inet_addr(ip);
    if(connect(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1){
        perror("connect() error: ");
        exit(0);
    } else { printf("SUCCESS!\n"); }
    fKey = handshake(&fd);
    //printf("%llx\n", fKey);

    pthread_create(
        &recvt,
        NULL,
        (void *)receivemsg,
        &fd
        );
    
    while(fgets(msg, 256, stdin) > 0) {
        strcpy(send_msg, client_name);
        strcat(send_msg, ":");
        strcat(send_msg, msg);
        if(*msg=='\n'){
            continue;
        }

        block* en_send_msg = EncryptString(send_msg, fKey);
        int en_length = GetDataSize(en_send_msg);

        if(write(fd, en_send_msg, en_length) < 0){
            printf("\nMessage was not sent\n");
        }
    }
    return 0;
}