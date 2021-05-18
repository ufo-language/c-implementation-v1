#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/d_tuple.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/mem.h"
#include "../src/object.h"

static void test_tupleN();
static void test_tupleFromArray();
static void test_tupleHashCode();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_tupleN", test_tupleN},
  {"test_tupleFromArray", test_tupleFromArray},
  {"test_tupleHashCode", test_tupleHashCode},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static Thread* thd;

static void test_before() {
  memStart();
  thd = threadNew();
  globalsSetup(thd);
}

static void test_after() {
  threadDelete(thd);
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_tuple() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_tupleN() { 
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);

  Object tup = tupleN(1, i100);
  ASSERT_EQ(1, tupleCount(tup));
  EXPECT_EQ(i100.a, tupleGet_unsafe(tup, 0).a);

  tup = tupleN(2, i100, i200);
  ASSERT_EQ(2, tupleCount(tup));
  EXPECT_EQ(i100.a, tupleGet_unsafe(tup, 0).a);
  EXPECT_EQ(i200.a, tupleGet_unsafe(tup, 1).a);

  tup = tupleN(3, i100, i200, i300);
  ASSERT_EQ(3, tupleCount(tup));
  EXPECT_EQ(i100.a, tupleGet_unsafe(tup, 0).a);
  EXPECT_EQ(i200.a, tupleGet_unsafe(tup, 1).a);
  EXPECT_EQ(i300.a, tupleGet_unsafe(tup, 2).a);
}

void test_tupleFromArray() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object ary = arrayNew(3);
  arraySet_unsafe(ary, 0, i100);
  arraySet_unsafe(ary, 1, i200);
  arraySet_unsafe(ary, 2, i300);
  Object tup = tupleFromArray(ary);
  ASSERT_EQ(3, tupleCount(tup));
  for (int n=0; n<tupleCount(tup); n++) {
    EXPECT_EQ(arrayGet_unsafe(ary, n).a, tupleGet_unsafe(tup, n).a);
  }
}

void test_tupleHashCode() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object tup = tupleN(3, i100, i200, i300);
  Word hashCode = objHashCode(tup);
  EXPECT_NE(0, hashCode);
}
