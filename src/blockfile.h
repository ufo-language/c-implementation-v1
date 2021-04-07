#include <stdio.h>

#include "defines.h"

FILE* blockFileInit(char* fileName, uint blockSize, uint nBlocks);
void blockFileClose(FILE* file);
void blockFileRead(byte* block, uint blockSize, uint blockNum, FILE* file);
void blockFileWrite(byte* block, uint blockSize, uint blockNum, FILE* file);
