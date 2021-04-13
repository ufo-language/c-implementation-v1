#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/d_tuple.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/mem.h"
#include "../src/object.h"

static void test_tuple1();
static void test_tuple2();
static void test_tuple3();
static void test_tupleFromArray();
static void test_tupleHashCode();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_tuple1", test_tuple1},
  {"test_tuple2", test_tuple2},
  {"test_tuple3", test_tuple3},
  {"test_tupleFromArray", test_tupleFromArray},
  {"test_tupleHashCode", test_tupleHashCode},
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

void test_tuple() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_tuple1() {
  Object i100 = intNew(100);
  Object tup = tuple1(i100);
  ASSERT_EQ(1, tupleCount(tup));
  EXPECT_EQ(i100.a, tupleGet(tup, 0).a);
}

void test_tuple2() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object tup = tuple2(i100, i200);
  ASSERT_EQ(2, tupleCount(tup));
  EXPECT_EQ(i100.a, tupleGet(tup, 0).a);
  EXPECT_EQ(i200.a, tupleGet(tup, 1).a);
}

void test_tuple3() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object tup = tuple3(i100, i200, i300);
  ASSERT_EQ(3, tupleCount(tup));
  EXPECT_EQ(i100.a, tupleGet(tup, 0).a);
  EXPECT_EQ(i200.a, tupleGet(tup, 1).a);
  EXPECT_EQ(i300.a, tupleGet(tup, 2).a);
}

void test_tupleFromArray() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object ary = arrayNew(3);
  arraySet(ary, 0, i100);
  arraySet(ary, 1, i200);
  arraySet(ary, 2, i300);
  Object tup = tupleFromArray(ary);
  ASSERT_EQ(3, tupleCount(tup));
  for (int n=0; n<tupleCount(tup); n++) {
    EXPECT_EQ(arrayGet(ary, n).a, tupleGet(tup, n).a);
  }
}

void test_tupleHashCode() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object tup = tuple3(i100, i200, i300);
  Word hashCode = objHashCode(tup);
  EXPECT_NE(0, hashCode);
}
