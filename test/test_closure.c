#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_binding.h"
#include "../src/d_closure.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/delegate.h"
#include "../src/e_abstr.h"
#include "../src/e_ident.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_closureApply();
static void test_closureClose();
static void test_closureLexEnv();
static void test_closureMark();
static void test_closureNew();

Object _close(Object rule, Object env);
bool _isMarked(RawBlock blk);
RawBlock objToRawBlock(Object obj);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_closureNew", test_closureNew},
  {"test_closureClose", test_closureClose},
  {"test_closureMark", test_closureMark},
  {"test_closureLexEnv", test_closureLexEnv},
  {"test_closureApply", test_closureApply},
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

void test_closure() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_closureNew() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object params1 = listNew(x, EMPTY_LIST);
  Object body1 = listNew(x, listNew(y, listNew(z, EMPTY_LIST)));
  Object abstr1 = abstrNew(params1, body1);
  Object params2 = listNew(y, EMPTY_LIST);
  Object body2 = listNew(z, listNew(y, listNew(x, EMPTY_LIST)));
  Object abstr2 = abstrNew(params2, body2);
  abstrSetNext(abstr1, abstr2);
  Object closure = closureNew(abstr1, EMPTY_LIST);

  const Word PARAMS_OFS = 0;
  const Word BODY_OFS = 1;
  const Word NEXT_OFS = 2;
  Object rule1 = closure;
  Object params1a = {objGetData(rule1, PARAMS_OFS)};
  EXPECT_EQ(params1.a, params1a.a);
  Object body1a = {objGetData(rule1, BODY_OFS)};
  EXPECT_EQ(body1.a, body1a.a);
  Object rule2 = {objGetData(rule1, NEXT_OFS)};
  ASSERT_NE(nullObj.a, rule2.a);
  Object params2a = {objGetData(rule2, PARAMS_OFS)};
  EXPECT_EQ(params2.a, params2a.a);
  Object body2a = {objGetData(rule2, BODY_OFS)};
  EXPECT_EQ(body2.a, body2a.a);
}

void test_closureClose() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object params = listNew(x, EMPTY_LIST);
  Object body = listNew(x, listNew(y, EMPTY_LIST));
  Object rule = abstrNew(params, body);

  Object xBinding = bindingNew(x, i100);
  Object yBinding = bindingNew(y, i200);
  Object env = listNew(xBinding, listNew(yBinding, EMPTY_LIST));

  Object lexEnv = _close(rule, env);
  ASSERT_EQ(1, listCount(lexEnv));
  Object binding = listLocate(lexEnv, y);
  EXPECT_EQ(y.a, bindingGetLhs(binding).a);
  EXPECT_EQ(i200.a, bindingGetRhs(binding).a);
}

void test_closureMark() {
  Object a = identNew("a");
  Object b = identNew("b");
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object params1 = listNew(x, EMPTY_LIST);
  Object body = listNew(x, listNew(y, listNew(z, EMPTY_LIST)));
  Object abstr1 = abstrNew(params1, body);
  Object params2 = listNew(y, EMPTY_LIST);
  Object abstr2 = abstrNew(params2, body);
  abstrSetNext(abstr1, abstr2);

  Object env = listNew(bindingNew(a, intNew(100)),
                 listNew(bindingNew(b, intNew(200)),
                   EMPTY_LIST));
  Object closure = closureNew(abstr1, env);

  objMark(closure);

  EXPECT_T(_isMarked(objToRawBlock(x)));
  EXPECT_T(_isMarked(objToRawBlock(y)));
  EXPECT_T(_isMarked(objToRawBlock(x)));
  EXPECT_T(_isMarked(objToRawBlock(params1)));
  EXPECT_T(_isMarked(objToRawBlock(body)));
  EXPECT_T(_isMarked(objToRawBlock(params2)));
  EXPECT_T(_isMarked(objToRawBlock(closure)));
}

void test_closureLexEnv() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object params1 = listNew(x, EMPTY_LIST);
  Object body1 = listNew(x, listNew(y, listNew(z, EMPTY_LIST)));
  Object abstr1 = abstrNew(params1, body1);
  Object params2 = listNew(y, EMPTY_LIST);
  Object body2 = listNew(z, listNew(y, listNew(x, EMPTY_LIST)));
  Object abstr2 = abstrNew(params2, body2);
  abstrSetNext(abstr1, abstr2);
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object env = listNew(bindingNew(x, i100),
                 listNew(bindingNew(y, i200),
                   listNew(bindingNew(z, i300),
                     EMPTY_LIST)));
  Object closure = closureNew(abstr1, env);

  const Word NEXT_OFS = 2;
  const Word LEXENV_OFS = 3;

  Object lexEnv1 = {objGetData(closure, LEXENV_OFS)};
  ASSERT_EQ(2, listCount(lexEnv1));
  Object yBinding = listLocate(lexEnv1, y);
  EXPECT_NE(nullObj.a, yBinding.a);
  Object zBinding = listLocate(lexEnv1, z);
  EXPECT_NE(nullObj.a, zBinding.a);

  Object rule2 = {objGetData(closure, NEXT_OFS)};
  Object lexEnv2 = {objGetData(rule2, LEXENV_OFS)};
  ASSERT_EQ(2, listCount(lexEnv2));
  Object xBinding = listLocate(lexEnv2, x);
  EXPECT_NE(nullObj.a, xBinding.a);
  zBinding = listLocate(lexEnv2, z);
  EXPECT_NE(nullObj.a, zBinding.a);
}

void test_closureApply() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object params1 = EMPTY_LIST;
  Object body1 = i100;
  Object abstr1 = abstrNew(params1, body1);

  Object params2 = listNew(x, EMPTY_LIST);
  Object body2 = i200;
  Object abstr2 = abstrNew(params2, body2);
  abstrSetNext(abstr1, abstr2);

  Object params3 = listNew(x, listNew(y, EMPTY_LIST));
  Object body3 = arrayNew(3);
  arraySet(body3, 0, x);
  arraySet(body3, 1, y);
  arraySet(body3, 2, z);
  Object abstr3 = abstrNew(params3, body3);
  abstrSetNext(abstr2, abstr3);

  Thread* thd = threadNew();
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);
  threadEnvBind(thd, z, i300);
  Object env = threadGetEnv(thd);
  Object closure = closureNew(abstr1, env);

  Object args1 = EMPTY_LIST;
  Object res1 = closureApply(closure, args1, thd);
  EXPECT_EQ(i100.a, res1.a);

  Object args2 = listNew(NOTHING, EMPTY_LIST);
  Object res2 = closureApply(closure, args2, thd);
  EXPECT_EQ(i200.a, res2.a);

  Object i400 = intNew(400);
  Object i500 = intNew(500);
  Object args3 = listNew(i400, listNew(i500, EMPTY_LIST));
  Object res3 = closureApply(closure, args3, thd);

  ASSERT_EQ(D_Array, objGetType(res3));
  ASSERT_EQ(3, arrayCount(res3));
  EXPECT_EQ(i400.a, arrayGet(res3, 0).a);
  EXPECT_EQ(i500.a, arrayGet(res3, 1).a);
  EXPECT_EQ(i300.a, arrayGet(res3, 2).a);

  threadDelete(thd);
}
