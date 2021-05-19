#include <stdio.h>

#include "test.h"

#include "../src/d_binding.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/mem.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_bindingNew();
static void test_bindingGetSet();
static void test_bindingEquals();
static void test_bindingEval();
static void test_bindingMatch();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_bindingNew", test_bindingNew},
  {"test_bindingGetSet", test_bindingGetSet},
  {"test_bindingEquals", test_bindingEquals},
  {"test_bindingEval", test_bindingEval},
  {"test_bindingMatch", test_bindingMatch},
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
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_binding() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_bindingNew() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object bnd = bindingNew(x, i100);
  ASSERT_NE(0, bnd.a);
}

void test_bindingGetSet() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object bnd = bindingNew(x, i100);

  EXPECT_EQ(x.a, bindingGetLhs(bnd).a);

  EXPECT_EQ(i100.a, bindingGetRhs(bnd).a);

  Object i200 = intNew(200);
  bindingSetRhs(bnd, i200);
  EXPECT_EQ(i200.a, bindingGetRhs(bnd).a);
}

void test_bindingEquals() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object bndx1 = bindingNew(x, i100);

  Object bndx2 = bindingNew(x, i100);
  EXPECT_T(objEquals(bndx1, bndx2, thd));

  Object i200 = intNew(200);
  bindingSetRhs(bndx2, i200);
  EXPECT_F(objEquals(bndx1, bndx2, thd));

  Object y = identNew("x");
  Object bndy1 = bindingNew(y, i200);

  EXPECT_F(bindingEquals(bndx1, bndy1, thd));
}

void test_bindingEval() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);

  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  Object bnd = bindingNew(x, y);

  Object res = eval(bnd, thd);
  EXPECT_NE(nullObj.a, res.a);
  EXPECT_EQ(D_Binding, objGetType(res));
  EXPECT_EQ(i100.a, bindingGetLhs(res).a);
  EXPECT_EQ(i200.a, bindingGetRhs(res).a);
}

void test_bindingMatch() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object bnd1 = bindingNew(x, y);

  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object bnd2 = bindingNew(i100, i200);

  Object bindingList = EMPTY_LIST;
  Object bindingList1 = objMatch(bnd1, bnd2, bindingList, thd);

  Object res = listLocate(bindingList1, x, thd);
  EXPECT_T(objEquals(bindingNew(x, i100), res, thd));
  res = listLocate(bindingList1, y, thd);
  EXPECT_T(objEquals(bindingNew(y, i200), res, thd));
}
