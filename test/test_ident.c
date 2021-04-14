#include <stdio.h>

#include "test.h"

#include "../src/d_int.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_identEval();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_identEval", test_identEval},
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

void test_ident() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_identEval() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Thread* thd = threadNew();
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);
  threadEnvBind(thd, z, i300);
  Object val = objEval(x, thd);
  EXPECT_EQ(i100.a, val.a);
  val = objEval(y, thd);
  EXPECT_EQ(i200.a, val.a);
  val = objEval(z, thd);
  EXPECT_EQ(i300.a, val.a);
}
