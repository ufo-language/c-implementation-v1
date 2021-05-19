#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/d_string.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_setNew();
static void test_setAddCount();
static void test_setHas();
static void test_setEqual();
static void test_setEval();
static void test_setRemove1();
static void test_setRemove2();
static void test_setToArray();

/* list the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_setNew", test_setNew},
  {"test_setAddCount", test_setAddCount},
  {"test_setHas", test_setHas},
  {"test_setEqual", test_setEqual},
  {"test_setEval", test_setEval},
  {"test_setRemove1", test_setRemove1},
  {"test_setRemove2", test_setRemove2},
  {"test_setToArray", test_setToArray},
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

void test_set() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_setNew() {
  Object set = setNew();

  EXPECT_EQ(D_Set, objGetType(set));

  Word nElems = objGetData(set, 0);
  Word loadingFactor = objGetData(set, 1);
  Object buckets = {objGetData(set, 2)};
  Word nBuckets = arrayCount(buckets);

  EXPECT_EQ(nBuckets, 8);
  EXPECT_EQ(nElems, 0);
  EXPECT_EQ(loadingFactor, 6);
  EXPECT_EQ(D_Array, objGetType(buckets));
  EXPECT_EQ(nBuckets, arrayCount(buckets));
}

void test_setAddCount() {
  Object x = identNew("x");
  Object i100 = intNew(100);

  Object set = setNew();
  setAddElem(set, x, thd);

  EXPECT_EQ(1, objGetData(set, 0)); /* nElems */
  EXPECT_EQ(1, setCount(set));
  EXPECT_T(setHas(set, x, thd));

  setAddElem(set, i100, thd);

  EXPECT_EQ(2, objGetData(set, 0)); /* nElems */
  EXPECT_EQ(2, setCount(set));
  EXPECT_T(setHas(set, i100, thd));

  setAddElem(set, x, thd);
  setAddElem(set, i100, thd);

  EXPECT_EQ(2, objGetData(set, 0)); /* nElems */
  EXPECT_EQ(2, setCount(set));
  EXPECT_T(setHas(set, i100, thd));
}

void test_setHas() {
  Object set = setNew();

  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");

  setAddElem(set, x, thd);
  setAddElem(set, y, thd);

  EXPECT_T(setHas(set, x, thd));
  EXPECT_T(setHas(set, y, thd));
  EXPECT_F(setHas(set, z, thd));
}

void test_setEqual() {
  Object set1 = setNew();
  Object set2 = setNew();
  EXPECT_T(objEquals(set1, set2, thd));

  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");

  setAddElem(set1, x, thd);
  setAddElem(set1, y, thd);
  setAddElem(set1, z, thd);

  setAddElem(set2, x, thd);
  setAddElem(set2, y, thd);

  EXPECT_F(objEquals(set1, set2, thd));

  setAddElem(set2, z, thd);

  EXPECT_T(objEquals(set2, set1, thd));
}

static void test_setEval() {
  Object set1 = setNew();

  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);

  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  setAddElem(set1, x, thd);
  setAddElem(set1, y, thd);

  Object set2 = objEval(set1, thd);

  EXPECT_T(setHas(set2, i100, thd));
  EXPECT_T(setHas(set2, i200, thd));
}

static void test_setRemove1() {
  Object set = setNew();

  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");

  setAddElem(set, x, thd);
  setAddElem(set, y, thd);
  setAddElem(set, z, thd);

  EXPECT_T(setHas(set, x, thd));
  EXPECT_T(setHas(set, y, thd));
  EXPECT_T(setHas(set, z, thd));

  setRemoveElem(set, x, thd);
  EXPECT_F(setHas(set, x, thd));
  EXPECT_T(setHas(set, y, thd));
  EXPECT_T(setHas(set, z, thd));

  setRemoveElem(set, y, thd);
  EXPECT_F(setHas(set, x, thd));
  EXPECT_F(setHas(set, y, thd));
  EXPECT_T(setHas(set, z, thd));

  setRemoveElem(set, z, thd);
  EXPECT_F(setHas(set, x, thd));
  EXPECT_F(setHas(set, y, thd));
  EXPECT_F(setHas(set, z, thd));
}

static void test_setRemove2() {
  Object set1 = setNew();

  /* 0 and 8 share the same bucket */
  Object i0 = intNew(0);
  Object i8 = intNew(8);
  setAddElem(set1, i0, thd);
  setAddElem(set1, i8, thd);

  setRemoveElem(set1, i0, thd);

  EXPECT_EQ(1, setCount(set1));
  EXPECT_F(setHas(set1, i0, thd));
  EXPECT_T(setHas(set1, i8, thd));
  
  Object set2 = setNew();

  /* 0 and 8 share the same bucket */
  setAddElem(set2, i0, thd);
  setAddElem(set2, i8, thd);

  setRemoveElem(set2, i8, thd);

  EXPECT_EQ(1, setCount(set1));
  EXPECT_T(setHas(set2, i0, thd));
  EXPECT_F(setHas(set2, i8, thd));
}

static void test_setToArray() {
  Object set = setNew();

  /* 0 and 8 sharea the same bucket */
  for (int n=0; n<4; n++) {
    setAddElem(set, intNew(n * 100), thd);
  }
  EXPECT_EQ(4, setCount(set));

  Object array = setToArray(set);
  EXPECT_EQ(4, arrayCount(array));
  for (int n=0; n<4; n++) {
    EXPECT_T(setHas(set, intNew(n * 100), thd));
  }
}
