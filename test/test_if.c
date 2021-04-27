#include <stdio.h>

#include "test.h"

#include "../src/d_bool.h"
#include "../src/d_int.h"
#include "../src/e_if.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_ifNew();
static void test_ifEval();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_ifNew", test_ifNew},
  {"test_ifEval", test_ifEval},
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

void test_if() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_ifNew() {
  Object obj1 = intNew(100);
  Object obj2 = intNew(200);
  Object obj3 = intNew(300);
  Object ifThen = ifNew(obj1, obj2, obj3);
  EXPECT_EQ(obj1.a, objGetData(ifThen, 0));
  EXPECT_EQ(obj2.a, objGetData(ifThen, 1));
  EXPECT_EQ(obj3.a, objGetData(ifThen, 2));
}

void test_ifEval() {
  Object t = boolNew(true);
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object ifThen1 = ifNew(t, i100, i200);
  Thread* thd = 0;
  Object res1 = ifEval(ifThen1, thd);
  EXPECT_EQ(res1.a, i100.a);

  Object f = boolNew(false);
  Object ifThen2 = ifNew(f, i100, i200);
  Object res2 = ifEval(ifThen2, thd);
  EXPECT_EQ(res2.a, i200.a);  
}
