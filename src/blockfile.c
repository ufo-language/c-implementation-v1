#include <stdio.h>
#include <string.h>

#include "blockfile.h"
#include "defines.h"

/* All block sizes are in bytes. */

FILE* blockFileInit(char* fileName, uint blockSize, uint nBlocks) {
  FILE* file = fopen(fileName, "w+b");
  char block[blockSize];
  memset(block, 0, blockSize);
  for (int n=0; n<nBlocks; n++) {
    fwrite(block, 1, blockSize, file);
  }
  fflush(file);
  fseek(file, 0, SEEK_SET);
  return file;
}

void blockFileClose(FILE* file) {
  fclose(file);
}

void blockFileRead(byte* block, uint blockSize, uint blockNum, FILE* file) {
  fseek(file, blockSize * blockNum, SEEK_SET);
  fread(block, 1, blockSize, file);
}

void blockFileWrite(byte* block, uint blockSize, uint blockNum, FILE* file) {
  fseek(file, blockSize * blockNum, SEEK_SET);
  fwrite(block, 1, blockSize, file);
}
