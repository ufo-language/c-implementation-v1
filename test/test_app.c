#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_closure.h"
#include "../src/d_list.h"
#include "../src/d_int.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_abstr.h"
#include "../src/e_app.h"
#include "../src/e_ident.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_appNew();
static void test_appEval();
static void test_appFreeVars();
static void test_appMark();

bool _isMarked(RawBlock blk);
RawBlock objToRawBlock(Object obj);

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_appNew", test_appNew},
  {"test_appEval", test_appEval},
  {"test_appFreeVars", test_appFreeVars},
  {"test_appMark", test_appMark},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static Thread* thd;

static void test_before() {
  memStart();
  thd = threadNew();
  globalsSetup(thd);
}

static void test_after() {
  threadDelete(thd);
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_app() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_appNew() {
  /* set up the closure */
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object params1 = EMPTY_LIST;
  Object body1 = listNew(i100, EMPTY_LIST);
  Object abstr1 = abstrNew(params1, body1);

  Object params2 = listNew(x, EMPTY_LIST);
  Object body2 = listNew(i200, EMPTY_LIST);
  Object abstr2 = abstrNew(params2, body2);
  abstrSetNext(abstr1, abstr2);

  Object params3 = listNew(x, listNew(y, EMPTY_LIST));
  Object body3 = listNew(arrayNew(3), EMPTY_LIST);
  arraySet_unsafe(body3, 0, x);
  arraySet_unsafe(body3, 1, y);
  arraySet_unsafe(body3, 2, z);
  Object abstr3 = abstrNew(params3, body3);
  abstrSetNext(abstr2, abstr3);

  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);
  threadEnvBind(thd, z, i300);
  Object env = threadGetEnv(thd);
  Object closure = closureNew(abstr1, env, thd);

  /* create the application */
  Object app = appNew(closure, EMPTY_LIST);

  const Word ABSTR_OFS = 0;
  const Word ARGS_OFS = 1;

  EXPECT_EQ(closure.a, objGetData(app, ABSTR_OFS));
  EXPECT_EQ(EMPTY_LIST.a, objGetData(app, ARGS_OFS));
}

void test_appEval() {
  /* set up the closure */
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object params1 = EMPTY_LIST;
  Object body1 = listNew(i100, EMPTY_LIST);
  Object abstr1 = abstrNew(params1, body1);

  Object params2 = listNew(x, EMPTY_LIST);
  Object body2 = listNew(i200, EMPTY_LIST);
  Object abstr2 = abstrNew(params2, body2);
  abstrSetNext(abstr1, abstr2);

  Object params3 = listNew(x, listNew(y, EMPTY_LIST));
  Object body3 = listNew(arrayNew(3), EMPTY_LIST);
  arraySet_unsafe(body3, 0, x);
  arraySet_unsafe(body3, 1, y);
  arraySet_unsafe(body3, 2, z);
  Object abstr3 = abstrNew(params3, body3);
  abstrSetNext(abstr2, abstr3);

  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);
  threadEnvBind(thd, z, i300);
  Object env = threadGetEnv(thd);
  Object closure = closureNew(abstr1, env, thd);

  /* create the application */
  Object app = appNew(closure, EMPTY_LIST);

  /* evaluate the application */
  Object res = eval(app, thd);

  EXPECT_EQ(i100.a, res.a);
}

void test_appFreeVars() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object app = appNew(x, y);

  Object freeVarSet = setNew();
  objFreeVars(app, freeVarSet, thd);

  EXPECT_EQ(2, setCount(freeVarSet));
  EXPECT_T(setHas(freeVarSet, x, thd));
  EXPECT_T(setHas(freeVarSet, y, thd));
}

void test_appMark() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object args = listNew(y, EMPTY_LIST);
  Object app = appNew(x, args);

  objMark(app);

  EXPECT_T(_isMarked(objToRawBlock(x)));
  EXPECT_T(_isMarked(objToRawBlock(y)));
  EXPECT_T(_isMarked(objToRawBlock(args)));
  EXPECT_T(_isMarked(objToRawBlock(app)));
}
