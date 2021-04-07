#include <stdio.h>

#include "test.h"

#include "../src/globals.h"
#include "../src/blockfile.h"

static void test_blockFileInit();
static void test_blockFileRead();
static void test_blockFileWrite();

static char* TEST_BLOCK_FILE_NAME = "__test__.blk";

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_blockFileInit", test_blockFileInit},
  {"test_blockFileRead", test_blockFileRead},
  {"test_blockFileWrite", test_blockFileWrite},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static void test_before() {
}

static void test_after() {
  remove(TEST_BLOCK_FILE_NAME);
}

/* Runs all the listed tests ---------------------------------------*/

void test_blockFile() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_blockFileInit() {
  uint blockSize = 16;
  uint nBlocks = 8;

  FILE* blockFile = blockFileInit(TEST_BLOCK_FILE_NAME, blockSize, nBlocks);
  blockFileClose(blockFile);
}

void test_blockFileRead() {
  uint blockSize = 16;
  uint nBlocks = 8;

  FILE* blockFile = blockFileInit(TEST_BLOCK_FILE_NAME, blockSize, nBlocks);
  byte block[blockSize];
  for (uint blockNum=0; blockNum<nBlocks; blockNum++) {
    blockFileRead(block, blockSize, blockNum, blockFile);
  }
  
  blockFileClose(blockFile);
}

void test_blockFileWrite() {
  uint blockSize = 16;
  uint nBlocks = 8;

  FILE* blockFile = blockFileInit(TEST_BLOCK_FILE_NAME, blockSize, nBlocks);
  byte block[blockSize];
  block[0] = 0xFE;
  block[blockSize - 1] = 0xFF;
  blockFileWrite(block, blockSize, 0, blockFile);
  blockFileRead(block, blockSize, 1, blockFile);
  EXPECT_EQ(0, block[0]);
  EXPECT_EQ(0, block[blockSize - 1]);
  blockFileRead(block, blockSize, 0, blockFile);
  EXPECT_EQ(0xFE, block[0]);
  EXPECT_EQ(0xFF, block[blockSize - 1]);
  
  blockFileClose(blockFile);
}
