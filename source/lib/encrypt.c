#include "encrypt.h"

block GeneratePrivateKey() {
    block PrivateKey = GenerateRandomKey();
    return PrivateKey;
}

block GenerateRandomKey() {
    block newKey;
    getrandom(&newKey, sizeof(block), GRND_RANDOM);
    return newKey;
}

block CalculatePublicKey(block inPrivateKey) {
    return modpow(POWER_BASE, inPrivateKey, MAX_NUM);
}

block CalculateFinalKey(block myPrivateKey, block inPublicKey) {
    return modpow(inPublicKey, myPrivateKey, MAX_NUM);
}

block* FitDataToBlocks(void* inData, int inDataLength) {
    void* newData;
    if (inDataLength%(sizeof(block))) {
        newData = calloc(sizeof(block), (inDataLength/(sizeof(block)))+2);
    } else {
        newData = calloc(sizeof(block), (inDataLength/(sizeof(block)))+1);
    }
    memcpy(newData, inData, inDataLength);
    return newData;
}

block* Encrypt(void* inData, int inDataLength, block inKey) {
    block* FitData = FitDataToBlocks(inData, inDataLength);
    int BlocksTotal = CountBlocks(FitData);
    for (int i = 0; i < BlocksTotal; i++) {
        BlockFunction(FitData+i, inKey);
    }
    return FitData;
}

int GetDataSize(block* inData) {
    return (CountBlocks(inData)+1)*sizeof(block);
}

int CountBlocks(block* inData) {
    block* i;
    for (i = inData; *i; i++);
    return (i-inData);
}

block* Decrypt(block* inData, block inKey) {
    int BlocksTotal = CountBlocks(inData);
    for (int i = 0; i < BlocksTotal; i++) {
        BlockFunction((void*)(inData+i), inKey);
    }
    return inData;
}

block* EncryptString(char* inString, block inKey) {
    block* Data = Encrypt((void*)inString, strlen(inString)+1, inKey);
    return Data;
}

char* DecryptString(block* inData, block inKey) {
    inData = Decrypt(inData, inKey);
    char* newString = calloc(sizeof(char),(strlen((char*)inData)+1));
    strcpy(newString,(char*)inData);
    return newString; 
}

void BlockFunction(block* inData, block inKey) {
    *inData = *inData ^ inKey;
}


block modpow(block A, block B, block C)
{
    // Base cases
    if (A == 0)
        return 0;
    if (B == 0)
        return 1;
 
    // If B is even
    unsigned __int128 y;
    if (B % 2 == 0) {
        y = modpow(A, B / 2, C);
        y = (y * y) % C;
    }
 
    // If B is odd
    else {
        y = A % C;
        y = (y * modpow(A, B - 1, C) % C) % C;
    }
 
    return ((y + C) % C);
}
 