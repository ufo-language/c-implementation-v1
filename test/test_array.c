#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_binding.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/globals.h"
#include "../src/mem.h"
#include "../src/object.h"

static void test_arrayNewSize();
static void test_arrayGet();
static void test_arraySet();
static void test_arrayCount();
static void test_arrayEquals();
static void test_arrayMatch();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_arrayNewSize", test_arrayNewSize},
  {"test_arrayGet", test_arrayGet},
  {"test_arraySet", test_arraySet},
  {"test_arrayCount", test_arrayCount},
  {"test_arrayEquals", test_arrayEquals},
  {"test_arrayMatch", test_arrayMatch},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static Thread* thd;

static void test_before() {
  memStart();
  globalsSetup();
  thd = threadNew();
}

static void test_after() {
  threadDelete(thd);
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_array() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_arrayNewSize() {
  Object ary1 = arrayNew(3);
  ASSERT_NE(0, ary1.a);
  EXPECT_EQ(3, arrayCount(ary1));
}

void test_arrayGet() {
  Object ary1 = arrayNew(3);
  ASSERT_NE(0, ary1.a);
  EXPECT_EQ(NOTHING.a, arrayGet_unsafe(ary1, 0).a);
  EXPECT_EQ(NOTHING.a, arrayGet_unsafe(ary1, 1).a);
  EXPECT_EQ(NOTHING.a, arrayGet_unsafe(ary1, 2).a);
}

void test_arraySet() {
  Object ary1 = arrayNew(3);
  ASSERT_NE(0, ary1.a);
  Object obj0 = intNew(100);
  Object obj1 = intNew(200);
  Object obj2 = intNew(300);
  arraySet_unsafe(ary1, 0, obj0);
  arraySet_unsafe(ary1, 1, obj1);
  arraySet_unsafe(ary1, 2, obj2);
  EXPECT_EQ(obj0.a, arrayGet_unsafe(ary1, 0).a);
  EXPECT_EQ(obj1.a, arrayGet_unsafe(ary1, 1).a);
  EXPECT_EQ(obj2.a, arrayGet_unsafe(ary1, 2).a);
}

void test_arrayCount() {
  Object ary0 = arrayNew(0);
  EXPECT_EQ(0, arrayCount(ary0));
  Object ary1 = arrayNew(1);
  EXPECT_EQ(1, arrayCount(ary1));
  Object ary2 = arrayNew(2);
  EXPECT_EQ(2, arrayCount(ary2));
}

void test_arrayEquals() {
  Object ary0a = arrayNew(0);
  Object ary0b = arrayNew(0);
  EXPECT_T(arrayEquals(ary0a, ary0b, thd));

  Object ary1a = arrayNew(1);
  Object ary1b = arrayNew(1);
  EXPECT_T(arrayEquals(ary1a, ary1b, thd));
  EXPECT_F(arrayEquals(ary0a, ary1a, thd));
  EXPECT_F(arrayEquals(ary1a, ary0a, thd));
  
  Object i100a = intNew(100);
  EXPECT_F(arrayEquals(ary1a, i100a, thd));
  arraySet_unsafe(ary1a, 0, i100a);
  EXPECT_F(arrayEquals(ary1a, ary1b, thd));
  EXPECT_F(arrayEquals(ary1b, ary1a, thd));
  Object i100b = intNew(100);
  arraySet_unsafe(ary1b, 0, i100b);
  EXPECT_T(arrayEquals(ary1a, ary1b, thd));
}

void test_arrayMatch() {
  Object bindingList = EMPTY_LIST;
  Object ary0a = arrayNew(0);
  Object ary0b = arrayNew(0);
  Object bindingList0 = objMatch(ary0a, ary0b, bindingList, thd);
  EXPECT_EQ(bindingList.a, bindingList0.a);

  Object ary1a = arrayNew(1);
  Object ary1b = arrayNew(1);
  Object bindingList1 = objMatch(ary1a, ary1b, bindingList, thd);
  EXPECT_EQ(bindingList.a, bindingList1.a);

  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object ary2a = arrayNew(2);
  arraySet_unsafe(ary2a, 0, x);
  arraySet_unsafe(ary2a, 1, i100);
  Object ary2b = arrayNew(2);
  arraySet_unsafe(ary2b, 0, i200);
  arraySet_unsafe(ary2b, 1, y);
  Object bindingList2 = objMatch(ary2a, ary2b, bindingList, thd);
  EXPECT_NE(bindingList.a, bindingList2.a);
  Object res = listLocate(bindingList2, x, thd);
  EXPECT_T(objEquals(bindingNew(x, i200), res, thd));
  res = listLocate(bindingList2, y, thd);
  EXPECT_T(objEquals(bindingNew(y, i100), res, thd));

  Object ary3 = arrayNew(3);
  Object ary4 = arrayNew(4);
  Object bindingList34 = objMatch(ary3, ary4, bindingList, thd);
  EXPECT_EQ(nullObj.a, bindingList34.a);
  bindingList34 = objMatch(ary4, ary3, bindingList, thd);
  EXPECT_EQ(nullObj.a, bindingList34.a);
}
