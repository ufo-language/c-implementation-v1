#include <stdio.h>

#include "test.h"

#include "../src/mem.h"

void test_memStart();
void test_memGettersSetters();
void test_memGetSetStatus();
void test_memAlloc();
void test_memFree1();
void test_memFree2();
void test_memFree3();

Word _getSize(RawBlock blk);
void _setSize(RawBlock blk, Word size);
RawBlock _getPrev(RawBlock blk);
void _setPrev(RawBlock blk, RawBlock prev);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_memStart", test_memStart},
  {"test_memGettersSetters", test_memGettersSetters},
  {"test_memGetSetStatus", test_memGetSetStatus},
  {"test_memAlloc", test_memAlloc},
  {"test_memFree1", test_memFree1},
  {"test_memFree2", test_memFree2},
  {"test_memFree3", test_memFree3},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static void test_before() {
  memStart();
}

static void test_after() {
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_mem() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

static Word maxMem = 65536 - 2 - 2*MEMBLK_OVERHEAD;
  
/*------------------------------------------------------------------*/
void test_memStart() {
  /* memStart() was already called by before() */
  EXPECT_EQ(2, memGetNBlocks());
  EXPECT_EQ(maxMem, memGetNFreeWords());
}

/*------------------------------------------------------------------*/
void test_memGettersSetters() {
  RawBlock blk1 = memAddrToRawBlock(1);
  _setSize(blk1, 4);
  EXPECT_EQ(4, _getSize(blk1));
  EXPECT_EQ(4, memGetStatus(blk1));
  memSetNext(blk1, memAddrToRawBlock(5));
  EXPECT_EQ(5, memGetNext(blk1).a)
  _setPrev(blk1, memAddrToRawBlock(6));
  EXPECT_EQ(6, _getPrev(blk1).a);
  memSetStatus(blk1, 3);
  EXPECT_EQ(3, memGetStatus(blk1));

  RawBlock blk2 = memAddrToRawBlock(64);
  Word size2 = 7;
  _setSize(blk2, size2);
  memSetNext(blk2, memAddrToRawBlock(1));
  _setPrev(blk2, memAddrToRawBlock(2));
  EXPECT_EQ(size2, memGetStatus(blk2));
  EXPECT_EQ(size2, vmemGet(blk2.a + MEMBLK_DATA_OFFSET + size2));
}

/*------------------------------------------------------------------*/
void test_memGetSetStatus() {
  Block blk1 = memAlloc(5);
  RawBlock rawBlk1 = memBlockToRawBlock(blk1);
  /* an allocated block has status 0 */
  EXPECT_EQ(0, memGetStatus(rawBlk1));

  memSetStatus(rawBlk1, 99);
  EXPECT_EQ(99, memGetStatus(rawBlk1));
}

/*------------------------------------------------------------------*/
void test_memAlloc() {
  Word expectedMaxMem = maxMem;
  EXPECT_EQ(expectedMaxMem, memGetNFreeWords());
  Word size1 = 16;
  Block blk1 = memAlloc(size1);
  EXPECT_EQ(size1, memSizeOf(blk1));
  RawBlock rawBlk1 = memBlockToRawBlock(blk1);
  EXPECT_EQ(0, memGetStatus(rawBlk1));
  expectedMaxMem -= size1 + MEMBLK_OVERHEAD;
  EXPECT_EQ(expectedMaxMem, memGetNFreeWords());

  Word size2 = 16;
  Block blk2 = memAlloc(size2);
  EXPECT_NE(blk1.a, blk2.a);
  EXPECT_EQ(size2, memSizeOf(blk2));
  RawBlock rawBlk2 = memBlockToRawBlock(blk2);
  EXPECT_EQ(0, memGetStatus(rawBlk2));
  expectedMaxMem -= size2 + MEMBLK_OVERHEAD;
  EXPECT_EQ(expectedMaxMem, memGetNFreeWords());
}

/*------------------------------------------------------------------*/
void test_memFree1() {
  Word freeMem1 = memGetNFreeWords();
  Block blk1 = memAlloc(16);
  Word freeMem2 = memGetNFreeWords();
  EXPECT_EQ(freeMem1 - (16 + MEMBLK_OVERHEAD), freeMem2);

  memFree(blk1);
  Word freeMem3 = memGetNFreeWords();
  EXPECT_EQ(freeMem1, freeMem3);
}

/*------------------------------------------------------------------*/
void test_memFree2() {
  EXPECT_EQ(2, memGetNBlocks());

  Word freeMem1 = memGetNFreeWords();

  Word size1 = 16;
  Block blk1 = memAlloc(size1);

  /* using the raw address of a block, the raw block's status after
     being allocated should be 0 */
  RawBlock rawBlk1 = memBlockToRawBlock(blk1);
  EXPECT_EQ(0, memGetStatus(rawBlk1));

  Word blk1RawSize = memSizeOf(blk1) + MEMBLK_OVERHEAD;
  EXPECT_EQ(freeMem1 - blk1RawSize, memGetNFreeWords());

  Word size2 = 24;
  Block blk2 = memAlloc(size2);
  Word blk2RawSize = memSizeOf(blk2) + MEMBLK_OVERHEAD;

  EXPECT_EQ(freeMem1 - blk1RawSize - blk2RawSize, memGetNFreeWords());

  memFree(blk1);
  /* after freeing, the status of a block should not be 0 */
  EXPECT_NE(0, memGetStatus(rawBlk1));
  
  memFree(blk2);
  RawBlock rawBlk2 = memBlockToRawBlock(blk2);
  EXPECT_NE(0, memGetStatus(rawBlk2));

  EXPECT_EQ(2, memGetNBlocks());
  EXPECT_EQ(freeMem1, memGetNFreeWords());
}

/*------------------------------------------------------------------*/
void test_memFree3() {
  EXPECT_EQ(2, memGetNBlocks());
  EXPECT_EQ(maxMem, memGetNFreeWords());

  Block blks[10];
  for (int n=0; n<10; n++) {
    blks[n] = memAlloc(n + 16);
  }

  /* free the blocks in a shuffled order */
  memFree(blks[2]);
  memFree(blks[5]);
  memFree(blks[4]);
  memFree(blks[6]);
  memFree(blks[9]);
  memFree(blks[3]);
  memFree(blks[7]);
  memFree(blks[1]);
  memFree(blks[0]);
  memFree(blks[8]);

  EXPECT_EQ(2, memGetNBlocks());
  EXPECT_EQ(maxMem, memGetNFreeWords());
}
