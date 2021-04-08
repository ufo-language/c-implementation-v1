#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_binding.h"
#include "../src/d_exn.h"
#include "../src/d_int.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/mem.h"

static void test_objMarkOnArray();
static void test_objMarkOnBinding();
static void test_objMarkOnClosure();
static void test_objMarkOnExn();

bool _isMarked(RawBlock blk);
RawBlock objToRawBlock(Object obj);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_objMarkOnArray", test_objMarkOnArray},
  {"test_objMarkOnBinding", test_objMarkOnBinding},
  {"test_objMarkOnClosure", test_objMarkOnClosure},
  {"test_objMarkOnExn", test_objMarkOnExn},
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

void test_objMarkOnArray() {
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

void test_objMarkOnBinding() {
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

void test_objMarkOnClosure() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object exn = exnNew(i200);

  EXPECT_F(_isMarked(objToRawBlock(i100)));
  EXPECT_F(_isMarked(objToRawBlock(i200)));
  EXPECT_F(_isMarked(objToRawBlock(i300)));
  EXPECT_F(_isMarked(objToRawBlock(exn)));

  /* exceptions don't use objMark_generic, but test exception marking anyway */
  objMark(exn);

  EXPECT_F(_isMarked(objToRawBlock(i100)));
  EXPECT_T(_isMarked(objToRawBlock(i200)));
  EXPECT_F(_isMarked(objToRawBlock(i300)));
  EXPECT_T(_isMarked(objToRawBlock(exn)));
}

void test_objMarkOnExn() {
  /* allocate some adjacent objects */
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object exn = exnNew(i200);

  EXPECT_F(_isMarked(objToRawBlock(i100)));
  EXPECT_F(_isMarked(objToRawBlock(i200)));
  EXPECT_F(_isMarked(objToRawBlock(i300)));
  EXPECT_F(_isMarked(objToRawBlock(exn)));

  /* exceptions don't use objMark_generic, but test exception marking anyway */
  objMark(exn);

  EXPECT_F(_isMarked(objToRawBlock(i100)));
  EXPECT_T(_isMarked(objToRawBlock(i200)));
  EXPECT_F(_isMarked(objToRawBlock(i300)));
  EXPECT_T(_isMarked(objToRawBlock(exn)));
}
