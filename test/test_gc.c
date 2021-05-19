#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_tuple.h"
#include "../src/delegate.h"
#include "../src/gc.h"
#include "../src/object.h"

static void test_gcMarkFlag();
static void test_gcMarkUnmark_integer();
static void test_gcMarkUnmark_list();
static void test_gcMarkUnmark_array();
static void test_gcMarkUnmark_tuple();
static void test_gcSweep1();
static void test_gcSweep2();
static void test_gcSweep3();
static void test_gcCommit1();

void _sweep();
void _unmark(RawBlock obj);
Word memGetStatus(RawBlock blk);
void memSetStatus(RawBlock blk, Word status);
void gcSetBlkMarkedFlag(RawBlock blk);
void gcSetObjMarkedFlag(Object obj);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_gcMarkFlag", test_gcMarkFlag},
  {"test_gcMarkUnmark_integer", test_gcMarkUnmark_integer},
  {"test_gcMarkUnmark_list", test_gcMarkUnmark_list},
  {"test_gcMarkUnmark_array", test_gcMarkUnmark_array},
  {"test_gcMarkUnmark_tupel", test_gcMarkUnmark_tuple},
  {"test_gcCommit1", test_gcCommit1},
  {"test_gcSweep1", test_gcSweep1},
  {"test_gcSweep2", test_gcSweep2},
  {"test_gcSweep3", test_gcSweep3},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static void test_before() {
  memStart();
  gcInit();
}

static void test_after() {
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_gc() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_gcMarkFlag() {
  Object i100 = intNew(100);
  RawBlock i100_raw = objToRawBlock(i100);
  EXPECT_F(gcIsMarked(i100));
  gcSetObjMarkedFlag(i100);
  EXPECT_T(gcIsMarked(i100));
  Word i100Status = memGetStatus(i100_raw);
  /* check idempotency of gcSetBlkMarkedFlag */
  gcSetBlkMarkedFlag(i100_raw);
  EXPECT_EQ(i100Status, memGetStatus(i100_raw));
}

void test_gcMarkUnmark_integer() {
  Object int1_obj = intNew(100);
  RawBlock int1_raw = objToRawBlock(int1_obj);
    
  EXPECT_EQ(0, memGetStatus(int1_raw));
  EXPECT_F(gcIsMarked(int1_obj));
  _unmark(int1_raw);
  EXPECT_EQ(0, memGetStatus(int1_raw));
  EXPECT_F(gcIsMarked(int1_obj));
  objMark(int1_obj);
  EXPECT_EQ(MEMBLK_GC_MARK, memGetStatus(int1_raw));
  EXPECT_T(gcIsMarked(int1_obj));
  _unmark(int1_raw);
  EXPECT_EQ(0, memGetStatus(int1_raw));
  EXPECT_F(gcIsMarked(int1_obj));

  /* test with non-zero status */
  Object int2_obj = intNew(100);
  RawBlock int2_raw = objToRawBlock(int2_obj);
  EXPECT_EQ(0, memGetStatus(int2_raw));
  EXPECT_F(gcIsMarked(int2_obj));
  _unmark(int2_raw);
  EXPECT_EQ(0, memGetStatus(int2_raw));
  EXPECT_F(gcIsMarked(int2_obj));
  objMark(int2_obj);
  EXPECT_EQ(MEMBLK_GC_MARK, memGetStatus(int2_raw));
  EXPECT_T(gcIsMarked(int2_obj));
  _unmark(int2_raw);
  EXPECT_EQ(0, memGetStatus(int2_raw));
  EXPECT_F(gcIsMarked(int2_obj));
}

void test_gcMarkUnmark_list() {
  Object int1_obj = intNew(100);
  //RawBlock int1_raw = objToRawBlock(int1_obj);
  Object int2_obj = intNew(200);
  //RawBlock int2_raw = objToRawBlock(int2_obj);

  Object list1_obj = listNew(int1_obj, int2_obj);
  //RawBlock list1_raw = objToRawBlock(list1_obj);

  EXPECT_F(gcIsMarked(int1_obj));
  EXPECT_F(gcIsMarked(int2_obj));
  EXPECT_F(gcIsMarked(list1_obj));

  objMark(list1_obj);

  EXPECT_T(gcIsMarked(list1_obj));
  EXPECT_T(gcIsMarked(int1_obj));
  EXPECT_T(gcIsMarked(int2_obj));
}

void test_gcMarkUnmark_array() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object array = arrayN(3, i100, i200, i300);

  EXPECT_F(gcIsMarked(i100));
  EXPECT_F(gcIsMarked(i200));
  EXPECT_F(gcIsMarked(i300));
  EXPECT_F(gcIsMarked(array));

  objMark(array);

  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
  EXPECT_T(gcIsMarked(i300));
  EXPECT_T(gcIsMarked(array));
}

void test_gcMarkUnmark_tuple() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object tuple = tupleN(3, i100, i200, i300);

  EXPECT_F(gcIsMarked(i100));
  EXPECT_F(gcIsMarked(i200));
  EXPECT_F(gcIsMarked(i300));
  EXPECT_F(gcIsMarked(tuple));

  objMark(tuple);

  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
  EXPECT_T(gcIsMarked(i300));
  EXPECT_T(gcIsMarked(tuple));
}

void test_gcCommit1() {
  Word freeBlocks0 = memGetNBlocks();
  Word freeWords0 = memGetNFreeWords();

  intNew(100);

  Word freeBlocks1 = memGetNBlocks();
  Word freeWords1 = memGetNFreeWords();

  EXPECT_EQ(freeBlocks0, freeBlocks1);
  EXPECT_GT(freeWords0, freeWords1);

  _sweep();

  Word freeBlocks2 = memGetNBlocks();
  Word freeWords2 = memGetNFreeWords();
  EXPECT_EQ(freeBlocks1, freeBlocks2);
  EXPECT_EQ(freeWords1, freeWords2);

  gcCommit();
  _sweep();

  Word freeBlocks3 = memGetNBlocks();
  Word freeWords3 = memGetNFreeWords();
  EXPECT_EQ(freeBlocks0, freeBlocks3);
  EXPECT_EQ(freeWords0, freeWords3);

  intNew(200);

  _sweep();
  
  Word freeBlocks4 = memGetNBlocks();
  Word freeWords4 = memGetNFreeWords();
  EXPECT_EQ(freeBlocks3, freeBlocks4);
  EXPECT_GT(freeWords3, freeWords4);

  gcCommit();
  _sweep();

  Word freeBlocks5 = memGetNBlocks();
  Word freeWords5 = memGetNFreeWords();
  EXPECT_EQ(freeBlocks3, freeBlocks5);
  EXPECT_EQ(freeWords3, freeWords5);
}

void test_gcSweep1() {
  Word initialBlocks = memGetNBlocks();
  Word initialMem = memGetNFreeWords();

  intNew(100);
  EXPECT_NE(initialMem, memGetNFreeWords());

  _sweep();
  EXPECT_NE(initialMem, memGetNFreeWords());
  
  gcCommit();
  _sweep();

  EXPECT_EQ(initialBlocks, memGetNBlocks());
  EXPECT_EQ(initialMem, memGetNFreeWords());
}

void test_gcSweep2() {
  Word initialBlocks = memGetNBlocks();
  Word initialMem = memGetNFreeWords();

  Object obj;
  for (int n=0; n<10; n++) {
    int size = rand() % 255 + 1;
    obj = arrayNew(size);
  }

  gcCommit();
  _sweep();

  EXPECT_EQ(initialBlocks, memGetNBlocks());
  EXPECT_EQ(initialMem, memGetNFreeWords());
}

void test_gcSweep3() {
  /* Allocate all of memory a few times over. This should trigger
     mark/sweep several times. */
  for (int n=0; n<256; n++) {
    arrayNew(2000);
    gcCommit();
  }
  EXPECT_T(true);
}
