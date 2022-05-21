#include <sys/random.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NUM 18446744073709551615
#define POWER_BASE 7

typedef unsigned long long int block;

block GenerateRandomKey();
block CalculatePublicKey(block inPrivateKey);
block CalculateFinalKey(block myPrivateKey, block inPublicKey);
block GeneratePrivateKey();

void BlockFunction(block* inData, block inKey);
block* Encrypt(void* inData, int inDataLength, block inKey);
block* Decrypt(block* inData, block inKey);
block* FitDataToBlocks(void* inData, int inDataLength);
int CountBlocks(block* inData);
int GetDataSize(block* inData);
block* EncryptString(char* inString, block inKey);
char* DecryptString(block* inData, block inKey);
block modpow(block A, block B, block C);