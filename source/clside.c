#include "clside.h"

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
    block *rcvdKey = receivekey(infd);
    block finalKey = CalculateFinalKey(privateKey, *rcvdKey);
    free(rcvdKey);
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
        free(deenmsg);
    }
    return NULL;
}

void handle_interrupt(int sig) {
    exit(0);
}

void closeeverything() {
    pthread_join(recvt, NULL);
    close(fd);
    printf("Процесс завершен.\n");
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
    if (*ip=='0') memcpy(ip,"0.0.0.0", 8);

    printf("Порт:\n>");
    scanf("%d",&port);
    if (port == 0) port=6969;
        
    printf("Ваш логин:\n>");
    scanf("%127s",name);
    if (*name=='0') memcpy(name,"anonymous", 10);

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

    signal(SIGINT, handle_interrupt); 

    while(1) {
        scanf("%255s", &(*msg));
        strcpy(send_msg, client_name);
        strcat(send_msg, ":");
        strcat(send_msg, msg);
        if(*msg=='\n'){
            continue;
        }
        block* en_send_msg = EncryptString(send_msg, fKey);
        int en_length = GetDataSize(en_send_msg);
        while(write(fd, en_send_msg, en_length) < 0){
            printf("Message was not sent\n");
        }
    }
    return 0;
}