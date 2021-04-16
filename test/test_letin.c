#include <stdio.h>

#include "test.h"

#include "../src/d_binding.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/e_letin.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_letInNew();
static void test_letInEval();
static void test_letInFreeVarsFreeBody();
static void test_letInFreeVarsBoundBody();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_letInNew", test_letInNew},
  {"test_letInEval", test_letInEval},
  {"test_letInFreeVarsFreeBody", test_letInFreeVarsFreeBody},
  {"test_letInFreeVarsBoundBody", test_letInFreeVarsBoundBody},
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

void test_letIn() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_letInNew() {
  Object letIn = letInNew(EMPTY_LIST, NOTHING);
  Object bindings = {objGetData(letIn, 0)};
  EXPECT_EQ(bindings.a, EMPTY_LIST.a);
  Object body = {objGetData(letIn, 1)};
  EXPECT_EQ(body.a, NOTHING.a);
}

void test_letInEval() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object binding = bindingNew(x, i100);
  Object bindings = listNew(binding, EMPTY_LIST);
  binding = bindingNew(y, i200);
  bindings = listNew(binding, bindings);
  Object letIn = letInNew(bindings, x);

  Thread* thd = threadNew();
  Object origEnv = threadGetEnv(thd);
  Object res = eval(letIn, thd);

  EXPECT_EQ(origEnv.a, threadGetEnv(thd).a);
  EXPECT_EQ(i100.a, res.a);

  letIn = letInNew(bindings, y);

  origEnv = threadGetEnv(thd);
  res = eval(letIn, thd);

  EXPECT_EQ(origEnv.a, threadGetEnv(thd).a);
  EXPECT_EQ(i200.a, res.a);
  threadDelete(thd);
}

void test_letInFreeVarsFreeBody() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object a = identNew("a");
  Object b = identNew("a");
  Object i100 = intNew(100);
  Object binding = bindingNew(x, i100);
  Object bindings = listNew(binding, EMPTY_LIST);
  binding = bindingNew(y, x);
  bindings = listNew(binding, bindings);
  binding = bindingNew(z, a);
  bindings = listNew(binding, bindings);
  Object letIn = letInNew(bindings, b);
  Object freeVarSet = setNew();

  objFreeVars(letIn, freeVarSet);
  EXPECT_EQ(1, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, a));
}

void test_letInFreeVarsBoundBody() {
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
  Object letIn = letInNew(bindings, x);
  Object freeVarSet = setNew();

  objFreeVars(letIn, freeVarSet);
  EXPECT_EQ(1, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, a));
}
