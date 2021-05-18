#include <stdio.h>

#include "test.h"

#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_abstr.h"
#include "../src/e_ident.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_abstrNew();
static void test_abstrFreeVars1();
static void test_abstrFreeVars2();
static void test_abstrFreeVars3();
static void test_abstrFreeVars4();
static void test_abstrMark();
static void test_abstrEval();

bool _isMarked(RawBlock blk);
RawBlock objToRawBlock(Object obj);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_abstrNew", test_abstrNew},
  {"test_abstrFreeVars1", test_abstrFreeVars1},
  {"test_abstrFreeVars2", test_abstrFreeVars2},
  {"test_abstrFreeVars3", test_abstrFreeVars3},
  {"test_abstrFreeVars4", test_abstrFreeVars4},
  {"test_abstrMark", test_abstrMark},
  {"test_abstrEval", test_abstrEval},
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

void test_abstr() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_abstrNew() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object params1 = listNew(x, EMPTY_LIST);
  Object body = listNew(x, listNew(y, listNew(z, EMPTY_LIST)));

  Object abstr1 = abstrNew(params1, body);

  Object params2 = listNew(y, EMPTY_LIST);
  Object abstr2 = abstrNew(params2, body);

  abstrSetNext(abstr1, abstr2);

  EXPECT_EQ(E_DoSeq, objGetType(abstrGetBody(abstr1)));
  EXPECT_EQ(params1.a, abstrGetParams(abstr1).a);
  EXPECT_EQ(abstr2.a, abstrGetNext(abstr1).a);

  EXPECT_EQ(E_DoSeq, objGetType(abstrGetBody(abstr2)));

  EXPECT_EQ(params2.a, abstrGetParams(abstr2).a);
  EXPECT_EQ(nullObj.a, abstrGetNext(abstr2).a);
}

void test_abstrFreeVars1() {
  Object x = identNew("x");
  Object params = listNew(x, EMPTY_LIST);
  Object body = listNew(x, EMPTY_LIST);
  Object abstr = abstrNew(params, body);

  Object freeVarSet = setNew();
  objFreeVars(abstr, freeVarSet);
  EXPECT_EQ(0, setCount(freeVarSet));
}

void test_abstrFreeVars2() {
  Object x = identNew("x");
  Object params = EMPTY_LIST;
  Object body = listNew(x, EMPTY_LIST);
  Object abstr = abstrNew(params, body);

  Object freeVarSet = setNew();
  objFreeVars(abstr, freeVarSet);
  EXPECT_EQ(1, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, x));
}

void test_abstrFreeVars3() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object params = listNew(x, EMPTY_LIST);
  Object body = listNew(x, listNew(y, EMPTY_LIST));
  Object abstr = abstrNew(params, body);

  Object freeVarSet = setNew();
  objFreeVars(abstr, freeVarSet);
  EXPECT_EQ(1, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, y));
}

void test_abstrFreeVars4() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object params1 = listNew(x, EMPTY_LIST);
  Object body = listNew(x, listNew(y, listNew(z, EMPTY_LIST)));
  Object abstr1 = abstrNew(params1, body);
  Object params2 = listNew(y, EMPTY_LIST);
  Object abstr2 = abstrNew(params2, body);
  abstrSetNext(abstr1, abstr2);

  Object freeVarSet = setNew();
  objFreeVars(abstr1, freeVarSet);
  EXPECT_EQ(3, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, x));
  EXPECT_T(setHas(freeVarSet, y));
  EXPECT_T(setHas(freeVarSet, z));
}

void test_abstrMark() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object params1 = listNew(x, EMPTY_LIST);
  Object body = listNew(x, listNew(y, listNew(z, EMPTY_LIST)));
  Object abstr1 = abstrNew(params1, body);
  Object params2 = listNew(y, EMPTY_LIST);
  Object abstr2 = abstrNew(params2, body);
  abstrSetNext(abstr1, abstr2);

  objMark(abstr1);

  EXPECT_T(_isMarked(objToRawBlock(x)));
  EXPECT_T(_isMarked(objToRawBlock(y)));
  EXPECT_T(_isMarked(objToRawBlock(x)));
  EXPECT_T(_isMarked(objToRawBlock(params1)));
  EXPECT_T(_isMarked(objToRawBlock(abstr1)));
  EXPECT_T(_isMarked(objToRawBlock(params2)));
  EXPECT_T(_isMarked(objToRawBlock(abstr2)));
}

void test_abstrEval() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object params = listNew(x, EMPTY_LIST);
  Object body = listNew(x, listNew(y, EMPTY_LIST));
  Object abstr = abstrNew(params, body);

  Thread* thd = threadNew();
  Object clo = objEval(abstr, thd);

  ASSERT_EQ(D_Closure, objGetType(clo));

  threadDelete(thd);
}
