#include <stdio.h>

#include "test.h"

#include "../src/d_binding.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/e_let.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_letNew();
static void test_letEvalSingle();
static void test_letEvalMultiple();
static void test_letFreeVars();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_letNew", test_letNew},
  {"test_letEvalSingle", test_letEvalSingle},
  {"test_letEvalMultiple", test_letEvalMultiple},
  {"test_letFreeVars", test_letFreeVars},
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

void test_let() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_letNew() {
  Object let = letNew(EMPTY_LIST);
  Object bindings = {objGetData(let, 0)};
  EXPECT_EQ(bindings.a, EMPTY_LIST.a);
}

void test_letEvalSingle() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object binding = bindingNew(x, i100);
  Object bindings = listNew(binding, EMPTY_LIST);
  Object let = letNew(bindings);

  eval(let, thd);

  Object bindingRes = threadEnvLocate(thd, x);
  EXPECT_NE(nullObj.a, bindingRes.a);
  Object val = bindingGetRhs(bindingRes);
  EXPECT_EQ(i100.a, val.a);
}

void test_letEvalMultiple() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object binding = bindingNew(x, i100);
  Object bindings = listNew(binding, EMPTY_LIST);
  binding = bindingNew(y, i200);
  bindings = listNew(binding, bindings);
  Object let = letNew(bindings);

  eval(let, thd);

  Object bindingRes = threadEnvLocate(thd, x);
  EXPECT_NE(nullObj.a, bindingRes.a);
  Object val = bindingGetRhs(bindingRes);
  EXPECT_EQ(i100.a, val.a);

  bindingRes = threadEnvLocate(thd, y);
  EXPECT_NE(nullObj.a, bindingRes.a);
  val = bindingGetRhs(bindingRes);
  EXPECT_EQ(i200.a, val.a);
}

void test_letFreeVars() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object a = identNew("a");
  Object i100 = intNew(100);
  Object binding = bindingNew(x, i100);
  Object bindings = listNew(binding, EMPTY_LIST);
  binding = bindingNew(y, x);
  bindings = listNew(binding, bindings);
  binding = bindingNew(z, a);
  bindings = listNew(binding, bindings);
  Object let = letNew(bindings);

  Object freeVarSet = setNew();

  objFreeVars(let, freeVarSet, thd);
  EXPECT_EQ(1, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, a, thd));
}
