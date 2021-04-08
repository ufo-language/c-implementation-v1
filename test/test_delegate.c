#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_binding.h"
#include "../src/d_int.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/mem.h"

static void test_objMarkDelegateOnArray();
static void test_objMarkDelegateOnBinding();

bool _isMarked(RawBlock blk);
RawBlock objToRawBlock(Object obj);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_objMarkDelegateOnArray", test_objMarkDelegateOnArray},
  {"test_objMarkDelegateOnBinding", test_objMarkDelegateOnBinding},
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

void test_objMarkDelegateOnArray() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object ary = arrayNew(2);
  arraySet(ary, 0, i100);
  arraySet(ary, 1, i200);

  EXPECT_F(_isMarked(objToRawBlock(i100)));
  EXPECT_F(_isMarked(objToRawBlock(i200)));
  EXPECT_F(_isMarked(objToRawBlock(ary)));

  objMark_generic(ary, ARY_ELEMS_OFS, arrayCount(ary));

  EXPECT_T(_isMarked(objToRawBlock(i100)));
  EXPECT_T(_isMarked(objToRawBlock(i200)));
  EXPECT_T(_isMarked(objToRawBlock(ary)));
}

void test_objMarkDelegateOnBinding() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object bnd = bindingNew(i100, i200);

  EXPECT_F(_isMarked(objToRawBlock(i100)));
  EXPECT_F(_isMarked(objToRawBlock(i200)));
  EXPECT_F(_isMarked(objToRawBlock(bnd)));

  objMark_generic(bnd, BND_LHS_OFS, 2);

  EXPECT_T(_isMarked(objToRawBlock(i100)));
  EXPECT_T(_isMarked(objToRawBlock(i200)));
  EXPECT_T(_isMarked(objToRawBlock(bnd)));
}
