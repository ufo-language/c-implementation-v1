#include <stdio.h>

#include "test.h"

#include "../src/d_symbol.h"
#include "../src/e_throw.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_threadNew();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_threadNew", test_threadNew},
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

void test_thread() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_threadNew() {
  Thread* thd = threadNew();
  EXPECT_NE(0, thd);
  threadDelete(thd);
}
