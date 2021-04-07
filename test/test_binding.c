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
static void test_bindingEqual();
static void test_bindingEval();
static void test_bindingMatch();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_bindingNew", test_bindingNew},
  {"test_bindingGetSet", test_bindingGetSet},
  {"test_bindingEqual", test_bindingEqual},
  {"test_bindingEval", test_bindingEval},
  {"test_bindingMatch", test_bindingMatch},
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

void test_bindingEqual() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object bndx1 = bindingNew(x, i100);

  Object bndx2 = bindingNew(x, i100);
  EXPECT_T(objEqual(bndx1, bndx2));

  Object i200 = intNew(200);
  bindingSetRhs(bndx2, i200);
  EXPECT_F(objEqual(bndx1, bndx2));

  Object y = identNew("x");
  Object bndy1 = bindingNew(y, i200);

  EXPECT_F(bindingEqual(bndx1, bndy1));
}

void test_bindingEval() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);

  Thread* thd = threadNew();
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  Object bnd = bindingNew(x, y);

  Object res = eval(bnd, thd);
  EXPECT_NE(nullObj.a, res.a);
  EXPECT_EQ(D_Binding, objGetType(res));
  EXPECT_EQ(i100.a, bindingGetLhs(res).a);
  EXPECT_EQ(i200.a, bindingGetRhs(res).a);
  threadDelete(thd);
}

void test_bindingMatch() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object bnd1 = bindingNew(x, y);

  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object bnd2 = bindingNew(i100, i200);

  Object bindingList = EMPTY_LIST;
  Object bindingList1 = objMatch(bnd1, bnd2, bindingList);

  Object res = listLocate(bindingList1, x);
  EXPECT_T(objEqual(bindingNew(x, i100), res));
  res = listLocate(bindingList1, y);
  EXPECT_T(objEqual(bindingNew(y, i200), res));
}
