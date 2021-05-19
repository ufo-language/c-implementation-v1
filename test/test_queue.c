#include <stdio.h>

#include "test.h"

#include "../src/d_int.h"
#include "../src/d_queue.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_queueNew();
static void test_queueEnqDeq();
static void test_queueEquals();
static void test_queueEval();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_queueNew", test_queueNew},
  {"test_queueEnqDeq", test_queueEnqDeq},
  {"test_queueEquals", test_queueEquals},
  {"test_queueEval", test_queueEval},
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

void test_queue() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_queueNew() {
  Object q = queueNew();
  EXPECT_EQ(0, queueCount(q));
  EXPECT_EQ(0, objGetData(q, 0));  /* count */
  EXPECT_EQ(EMPTY_LIST.a, objGetData(q, 1));  /* head */
  EXPECT_EQ(EMPTY_LIST.a, objGetData(q, 2));  /* tail */
}

void test_queueEnqDeq() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);

  Object q = queueNew();
  EXPECT_EQ(0, queueCount(q));
  queueEnq(q, i100);
  EXPECT_EQ(1, queueCount(q));
  queueEnq(q, i200);
  EXPECT_EQ(2, queueCount(q));
  queueEnq(q, i300);
  EXPECT_EQ(3, queueCount(q));

  Object elem = queueDeq_unsafe(q);
  EXPECT_EQ(i100.a, elem.a);
  EXPECT_EQ(2, queueCount(q));
  elem = queueDeq_unsafe(q);
  EXPECT_EQ(i200.a, elem.a);
  EXPECT_EQ(1, queueCount(q));
  elem = queueDeq_unsafe(q);
  EXPECT_EQ(i300.a, elem.a);
  EXPECT_EQ(0, queueCount(q));

  EXPECT_EQ(EMPTY_LIST.a, objGetData(q, 1));  /* head */
  EXPECT_EQ(EMPTY_LIST.a, objGetData(q, 2));  /* tail */
}

void test_queueEquals() {
  Object i100 = intNew(100);

  Object q1 = queueNew();
  Object q2 = queueNew();
  EXPECT_T(queueEquals(q1, q2, thd));

  queueEnq(q1, i100);
  EXPECT_F(queueEquals(q1, q2, thd));

  queueEnq(q2, i100);
  EXPECT_T(queueEquals(q1, q2, thd));
}

void test_queueEval() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);

  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  Object q = queueNew();
  queueEnq(q, x);
  queueEnq(q, y);
  Object q1 = objEval(q, thd);

  EXPECT_EQ(2, queueCount(q1));
  EXPECT_EQ(i100.a, queueDeq_unsafe(q1).a);
  EXPECT_EQ(i200.a, queueDeq_unsafe(q1).a);
}
