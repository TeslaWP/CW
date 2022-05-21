#include "clside.h"
#include "lib/encrypt.h"

char msg[256];

block *receivekey(void *me){
    int sock = *((int*)me);
    char *keymsg;
    keymsg = malloc(sizeof(block));
    recv(sock, keymsg, sizeof(block), 0);
    return (block *)keymsg;
}

void *sendkey(void *fd, block publicKey){
    int sock = *((int*)fd);
    char pK[sizeof(block)];
    memcpy(pK, &publicKey, sizeof(block));
    write(sock, pK, strlen(pK));
    return NULL;
}

block handshake(void *fd){
    block privateKey = GeneratePrivateKey();
    block publicKey = CalculatePublicKey(privateKey);
    sendkey(fd, publicKey);
    block rcvdKey = *receivekey(fd);
    block finalKey = CalculateFinalKey(privateKey, rcvdKey);
    return finalKey;
}

void *receivemsg(void *me){
    int sock = *((int*)me);
    int len;
    while((len = recv(sock, msg, 256, 0)) > 0) {
        msg[len] = '\0';
        fputs(msg, stdout);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    struct sockaddr_in serv;
    int fd;
    pthread_t recvt;
    char send_msg[256];
    char client_name[32];
    char name[32];
    char ip[128];

    printf("Адрес сервера:\n>");
    scanf("%31s",ip);

    printf("Ваш логин:\n>");
    scanf("%127s",name);

    strcpy(client_name, name);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(1337);
    serv.sin_addr.s_addr = inet_addr(ip);
    if(connect(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1){
        perror("connect() error: ");
        exit(0);
    } else { printf("SUCCESS!\n"); }
    block fKey = handshake(&fd);
    printf("%llx\n", fKey);

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
        if(write(fd, send_msg, strlen(send_msg)) < 0){
            printf("\nMessage was not sent\n");
        }
    }
    pthread_join(recvt, NULL);
    close(fd);
    return 0;
}