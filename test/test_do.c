#include <stdio.h>

#include "test.h"

#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_do.h"
#include "../src/e_ident.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_doNew();
static void test_doEvalEmpty();
static void test_doEvalSingle();
static void test_doEvalMultiple();
static void test_doFreeVars();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_doNew", test_doNew},
  {"test_doEvalEmpty", test_doEvalEmpty},
  {"test_doEvalSingle", test_doEvalSingle},
  {"test_doEvalMultiple", test_doEvalMultiple},
  {"test_doFreeVars", test_doFreeVars},
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

void test_do() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_doNew() {
  Object seq1 = doNew(EMPTY_LIST);
  Object data = {objGetData(seq1, 0)};
  EXPECT_EQ(data.a, EMPTY_LIST.a);
}

void test_doEvalEmpty() {
  Object seq = doNew(EMPTY_LIST);
  Thread* thd = threadNew();
  Object res = eval(seq, thd);
  EXPECT_EQ(res.a, NOTHING.a);
}

void test_doEvalSingle() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Thread* thd = threadNew();
  threadEnvBind(thd, x, i100);

  Object elems = listNew(x, EMPTY_LIST);
  Object seq = doNew(elems);
  Object res = eval(seq, thd);
  EXPECT_EQ(res.a, i100.a);
  threadDelete(thd);
}

void test_doEvalMultiple() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Thread* thd = threadNew();
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  Object elems = listNew(y, EMPTY_LIST);
  elems = listNew(x, elems);
  Object seq = doNew(elems);
  Object res = eval(seq, thd);
  EXPECT_EQ(res.a, i200.a);
  threadDelete(thd);
}

void test_doFreeVars() {
  Object x = identNew("x");
  Object y = identNew("y");

  Object elems = listNew(y, EMPTY_LIST);
  elems = listNew(x, elems);
  Object seq = doNew(elems);
  Object freeVarSet = setNew();
  objFreeVars(seq, freeVarSet);
  EXPECT_EQ(2, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, x));
  EXPECT_T(setHas(freeVarSet, y));
}
