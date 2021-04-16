#include <stdio.h>

#include "test.h"

#include "../src/d_symbol.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/e_throw.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_throwNew();
static void test_throwEval();
static void test_throwFreeVars();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_throwNew", test_throwNew},
  {"test_throwEval", test_throwEval},
  {"test_throwFreeVars", test_throwFreeVars},
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

void test_throw() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_throwNew() {
  Object errSym = symbolNew("TestErrorSymbol");
  Object thrw = throwNew(errSym);

  EXPECT_NE(nullObj.a, thrw.a);
  EXPECT_EQ(errSym.a, objGetData(thrw, 0));
}

void test_throwEval() {
  Object errSym = symbolNew("TestErrorSymbol");
  Object thrw = throwNew(errSym);

  Thread* thd = threadNew();
  /*Object res =*/ evaluate(thrw, thd);
  printf("******** test_throwEval is not finished\n");

  threadDelete(thd);
}

void test_throwFreeVars() {
  Object x = identNew("x");
  Object thrw = throwNew(x);
  Object freeVarSet = setNew();
  objFreeVars(thrw, freeVarSet);

  EXPECT_EQ(1, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, x));
}
