#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/mem.h"

static void test_delegate1();

bool _isMarked(RawBlock blk);
RawBlock objToRawBlock(Object obj);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_delegate1", test_delegate1},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static void test_before() {
  memStart();
  globalsSetup();
}

static void test_after() {
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_delegate() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void objMark_generic(Object obj, Word from, Word to);
void gcMark(Object obj);

void test_delegate1() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object ary1 = arrayNew(2);
  arraySet(ary1, 0, i100);
  arraySet(ary1, 1, i200);

  Word nElems = objGetData(ary1, 0);
  EXPECT_EQ(2, nElems);
  Object obj1 = {objGetData(ary1, 1)};
  Object obj2 = {objGetData(ary1, 2)};
  EXPECT_EQ(i100.a, obj1.a);
  EXPECT_EQ(i200.a, obj2.a);

  EXPECT_F(_isMarked(objToRawBlock(i100)));
  EXPECT_F(_isMarked(objToRawBlock(i200)));
  EXPECT_F(_isMarked(objToRawBlock(ary1)));

  //objMark_generic(ary1, 1, 2);
  objMark_generic(ary1, ARY_ELEMS_OFS, arrayCount(ary1));

  EXPECT_T(_isMarked(objToRawBlock(i100)));
  EXPECT_T(_isMarked(objToRawBlock(i200)));
  EXPECT_T(_isMarked(objToRawBlock(ary1)));
}
