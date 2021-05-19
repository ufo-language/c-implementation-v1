#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_evalListLocate();
static void test_evalIdent();
static void test_evalArray();
static void test_evalList();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_evalIdent", test_evalIdent},
  {"test_evalListLocate", test_evalListLocate},
  {"test_evalArray", test_evalArray},
  {"test_evalList", test_evalList},
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

void test_eval() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_evalListLocate() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);
  Object env = threadGetEnv(thd);

  Object xBinding = listLocate(env, x, thd);
  EXPECT_NE(0, xBinding.a);
  EXPECT_EQ(D_Binding, objGetType(xBinding));
  EXPECT_EQ(x.a, listGetFirst(xBinding).a);
  EXPECT_EQ(i100.a, listGetRest(xBinding).a);

  Object yBinding = listLocate(env, y, thd);
  EXPECT_NE(0, yBinding.a);
  EXPECT_EQ(D_Binding, objGetType(yBinding));
  EXPECT_EQ(y.a, listGetFirst(yBinding).a);
  EXPECT_EQ(i200.a, listGetRest(yBinding).a);

  Object zBinding = listLocate(env, z, thd);
  EXPECT_EQ(0, zBinding.a);
}

void test_evalIdent() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  Object res1 = eval(x, thd);
  EXPECT_EQ(i100.a, res1.a);

  Object res2 = eval(y, thd);
  EXPECT_EQ(i200.a, res2.a);
}

void test_evalArray() {
  /* set up the environment */
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  /* build the array */
  Object ary1 = arrayNew(2);
  arraySet_unsafe(ary1, 0, x);
  arraySet_unsafe(ary1, 1, y);
  Object res1 = eval(ary1, thd);
  EXPECT_T(objEquals(i100, arrayGet_unsafe(res1, 0), thd));
  EXPECT_T(objEquals(i200, arrayGet_unsafe(res1, 1), thd));
}

void test_evalList() {
  /* set up the environment */
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  /* build the list */
  Object list1 = listNew(x, y);
  Object res1 = eval(list1, thd);
  EXPECT_T(objEquals(i100, listGetFirst(res1), thd));
  EXPECT_T(objEquals(i200, listGetRest(res1), thd));
}
