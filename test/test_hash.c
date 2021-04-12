#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_hash.h"
#include "../src/d_int.h"
#include "../src/d_set.h"
#include "../src/d_string.h"
#include "../src/d_symbol.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/eval.h"
#include "../src/globals.h"
#include "../src/hash.h"
#include "../src/object.h"
#include "../src/thread.h"

static void test_hashCode();
static void test_hashCount();
static void test_hashEqual();
static void test_hashEval();
static void test_hashFreeVars();
static void test_hashGet();
static void test_hashNew();
static void test_hashPut1();
static void test_hashPut2();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_hashCode", test_hashCode},
  {"test_hashNew", test_hashNew},
  {"test_hashPut1", test_hashPut1},
  {"test_hashPut2", test_hashPut2},
  {"test_hashCount", test_hashCount},
  {"test_hashGet", test_hashGet},
  {"test_hashEqual", test_hashEqual},
  {"test_hashEval", test_hashEval},
  {"test_hashFreeVars", test_hashFreeVars},
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

void test_hash() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_hashCode() {
  Object s1 = stringNew("abc");
  Object s2 = stringNew("cab");
  Object s3 = stringNew("bca");
  Object s4 = stringNew("abc");
  
  Word sh1 = stringHash(s1);
  Word sh2 = stringHash(s2);
  Word sh3 = stringHash(s3);
  Word sh4 = stringHash(s4);

  EXPECT_NE(sh1, sh2);
  EXPECT_NE(sh2, sh3);
  EXPECT_EQ(sh1, sh4);

  Object i1 = identNew("abc");
  Object i2 = identNew("cab");
  Object i3 = identNew("bca");
  Object i4 = identNew("abc");

  Word ih1 = identHash(i1);
  Word ih2 = identHash(i2);
  Word ih3 = identHash(i3);
  Word ih4 = identHash(i4);

  EXPECT_NE(ih1, ih2);
  EXPECT_NE(ih2, ih3);
  EXPECT_EQ(ih1, ih4);

  EXPECT_NE(sh1, ih1);
  EXPECT_NE(sh2, ih2);
  EXPECT_NE(sh3, ih3);
  EXPECT_NE(sh4, ih4);
}

void test_hashNew() {
  Object hash = hashNew();

  Word nBindings = objGetData(hash, 0);
  Word loadingFactor = objGetData(hash, 1);
  Object buckets = {objGetData(hash, 2)};
  Word nBuckets = arrayCount(buckets);

  EXPECT_EQ(nBuckets, 8);
  EXPECT_EQ(nBindings, 0);
  EXPECT_EQ(loadingFactor, 6);
  EXPECT_EQ(D_Array, objGetType(buckets));
  EXPECT_EQ(nBuckets, arrayCount(buckets));
}

void test_hashPut1() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object i200 = intNew(200);

  Object hash = hashNew();
  hashPut(hash, x, i100);

  EXPECT_EQ(1, objGetData(hash, 0)); /* nBindings */
  EXPECT_EQ(1, hashCount(hash));
  EXPECT_EQ(i100.a, hashGet(hash, x).a);

  hashPut(hash, x, i200);

  EXPECT_EQ(1, objGetData(hash, 0)); /* nBindings */
  EXPECT_EQ(1, hashCount(hash));
  EXPECT_EQ(i200.a, hashGet(hash, x).a);
}

void test_hashPut2() {
  Object hash = hashNew();

  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);

  hashPut(hash, x, i100);
  hashPut(hash, y, i200);
  hashPut(hash, z, i300);

  Word nBindings = objGetData(hash, 0);
  Word loadingFactor = objGetData(hash, 1);
  Object buckets = {objGetData(hash, 2)};
  Word nBuckets = arrayCount(buckets);

  EXPECT_EQ(nBuckets, 8);
  EXPECT_EQ(nBindings, 3);
  EXPECT_EQ(loadingFactor, 6);
  EXPECT_EQ(D_Array, objGetType(buckets));
  EXPECT_EQ(nBuckets, arrayCount(buckets));

  Object a = identNew("a");
  Object b = identNew("b");
  Object c = identNew("c");
  Object d = identNew("d");
  Object i400 = intNew(400);
  Object i500 = intNew(500);
  Object i600 = intNew(600);
  Object i700 = intNew(700);

  hashPut(hash, a, i400);
  hashPut(hash, b, i500);
  hashPut(hash, c, i600);
  hashPut(hash, d, i700);
}

void test_hashCount() {
  Object hash = hashNew();

  EXPECT_EQ(0, hashCount(hash));

  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);

  hashPut(hash, x, i100);
  EXPECT_EQ(1, hashCount(hash));
  hashPut(hash, y, i200);
  EXPECT_EQ(2, hashCount(hash));
  hashPut(hash, z, i300);
  EXPECT_EQ(3, hashCount(hash));

  hashPut(hash, x, NOTHING);
  EXPECT_EQ(3, hashCount(hash));
  hashPut(hash, y, NOTHING);
  EXPECT_EQ(3, hashCount(hash));
  hashPut(hash, z, NOTHING);
  EXPECT_EQ(3, hashCount(hash));
}

void test_hashGet() {
  Object hash = hashNew();

  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);

  hashPut(hash, x, i100);
  hashPut(hash, y, i200);
  hashPut(hash, z, i300);

  EXPECT_EQ(i100.a, hashGet(hash, x).a);
  EXPECT_EQ(i200.a, hashGet(hash, y).a);
  EXPECT_EQ(i300.a, hashGet(hash, z).a);
}

void test_hashEqual() {
  Object hash1 = hashNew();
  Object hash2 = hashNew();
  EXPECT_T(objEquals(hash1, hash2));

  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);

  hashPut(hash1, x, i100);
  hashPut(hash1, y, i200);
  hashPut(hash1, z, i300);

  hashPut(hash2, x, i100);
  hashPut(hash2, y, i200);
  hashPut(hash2, z, i300);

  EXPECT_T(objEquals(hash1, hash2));
  EXPECT_T(objEquals(hash2, hash1));

  hashPut(hash1, x, i200);

  EXPECT_F(objEquals(hash1, hash2));
  EXPECT_F(objEquals(hash2, hash1));
}

static void test_hashEval() {
  Object hash1 = hashNew();

  Object a = identNew("a");
  Object b = identNew("b");
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);

  Thread* thd = threadNew();
  threadEnvBind(thd, x, i100);
  threadEnvBind(thd, y, i200);

  hashPut(hash1, a, x);
  hashPut(hash1, b, y);

  Object hash2 = objEval(hash1, thd);

  EXPECT_EQ(i100.a, hashGet(hash2, a).a);
  EXPECT_EQ(i200.a, hashGet(hash2, b).a);
  threadDelete(thd);
}

static void test_hashFreeVars() {
  Object hash1 = hashNew();

  Object ia = identNew("a");
  Object ib = identNew("b");
  Object ic = identNew("c");
  Object id = identNew("d");

  Object sa = symbolNew("A");
  Object sb = symbolNew("B");
  Object sc = symbolNew("C");
  Object sd = symbolNew("D");
  Object se = symbolNew("E");
  Object sf = symbolNew("F");

  hashPut(hash1, ia, sa);
  hashPut(hash1, ib, sb);
  hashPut(hash1, sc, ic);
  hashPut(hash1, sd, id);
  hashPut(hash1, se, sf);
  EXPECT_EQ(5, hashCount(hash1));

  Object freeVarSet = setNew();
  EXPECT_EQ(0, setCount(freeVarSet));

  objFreeVars(hash1, freeVarSet);
  EXPECT_EQ(4, setCount(freeVarSet));
}
