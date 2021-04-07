#include <stdio.h>
#include <string.h>

#include "test.h"

#include "../src/d_int.h"
#include "../src/gc.h"
#include "../src/object.h"

static void test_allocFree();
static void test_getSetType();
static void test_getSetData();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_allocFree", test_allocFree},
  {"test_getSetType", test_getSetType},
  {"test_getSetData", test_getSetData},
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

void test_object() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_allocFree() {
  Word initialBlocks = memGetNBlocks();
  Word initialMem = memGetNFreeWords();

  Object int1 = intNew(100);
  EXPECT_GT(initialMem, memGetNFreeWords());

  objFree(int1);

  EXPECT_EQ(initialBlocks, memGetNBlocks());
  EXPECT_EQ(initialMem, memGetNFreeWords());
}

void test_getSetType() {
  Object int1 = intNew(100);
  EXPECT_EQ(D_Int, objGetType(int1));
}

void test_getSetData() {
  Object int1 = intNew(100);
  EXPECT_EQ(100, objGetData(int1, 0));

  objSetData(int1, 0, 234);
  EXPECT_EQ(234, objGetData(int1, 0));
}
