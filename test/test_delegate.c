#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_binding.h"
#include "../src/d_closure.h"
#include "../src/d_exn.h"
#include "../src/d_hash.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_queue.h"
#include "../src/d_set.h"
#include "../src/delegate.h"
#include "../src/e_abstr.h"
#include "../src/e_app.h"
#include "../src/e_ident.h"
#include "../src/e_if.h"
#include "../src/e_let.h"
#include "../src/e_letin.h"
#include "../src/e_seq.h"
#include "../src/e_throw.h"
#include "../src/gc.h"
#include "../src/globals.h"
#include "../src/mem.h"

static void test_objMarkArray();
static void test_objMarkBinding();
static void test_objMarkClosure();
static void test_objMarkExn();
static void test_objMarkHash();
static void test_objMarkList();
static void test_objMarkQueue();
static void test_objMarkSet();
static void test_objMarkAbstr1();
static void test_objMarkAbstr2();
static void test_objMarkApp();
static void test_objMarkIf();
static void test_objMarkLet();
static void test_objMarkLetIn();
static void test_objMarkSeq();
static void test_objMarkThrow();
static void test_objCopy();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_objMarkArray", test_objMarkArray},
  {"test_objMarkBinding", test_objMarkBinding},
  {"test_objMarkClosure", test_objMarkClosure},
  {"test_objMarkExn", test_objMarkExn},
  {"test_objMarkHash", test_objMarkHash},
  {"test_objMarkList", test_objMarkList},
  {"test_objMarkQueue", test_objMarkQueue},
  {"test_objMarkSet", test_objMarkSet},
  {"test_objMarkAbstr1", test_objMarkAbstr1},
  {"test_objMarkAbstr2", test_objMarkAbstr2},
  {"test_objMarkApp", test_objMarkApp},
  {"test_objMarkIf", test_objMarkIf},
  {"test_objMarkLet", test_objMarkLet},
  {"test_objMarkLetIn", test_objMarkLetIn},
  {"test_objMarkSeq", test_objMarkSeq},
  {"test_objMarkThrow", test_objMarkThrow},
  {"test_objCopy", test_objCopy},
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

void test_delegate() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void objMark_generic(Object obj, Word from, Word to);

void test_objMarkArray() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object ary = arrayNew(2);
  arraySet(ary, 0, i100);
  arraySet(ary, 1, i200);

  EXPECT_F(gcIsMarked(i100));
  EXPECT_F(gcIsMarked(i200));
  EXPECT_F(gcIsMarked(ary));

  objMark(ary);

  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
  EXPECT_T(gcIsMarked(ary));
}

void test_objMarkBinding() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object bnd = bindingNew(i100, i200);

  EXPECT_F(gcIsMarked(i100));
  EXPECT_F(gcIsMarked(i200));
  EXPECT_F(gcIsMarked(bnd));

  objMark(bnd);

  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
  EXPECT_T(gcIsMarked(bnd));
}

void test_objMarkClosure() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object params = listNew(x, EMPTY_LIST);
  Object i100 = intNew(100);
  Object abstr = abstrNew(params, y);
  Object lexEnv = listNew(bindingNew(y, i100), EMPTY_LIST);
  Object clo = closureNew(abstr, lexEnv);
  /* the lexical environment created in the closure is not the same
     data structure as the lexical environment that was used to
     construct the closure */
  lexEnv.a = objGetData(clo, CLO_LEXENV_OFS);

  EXPECT_F(gcIsMarked(x));
  EXPECT_F(gcIsMarked(y));
  EXPECT_F(gcIsMarked(params));
  EXPECT_F(gcIsMarked(i100));
  EXPECT_F(gcIsMarked(lexEnv));
  EXPECT_F(gcIsMarked(clo));

  objMark(clo);

  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(y));
  EXPECT_T(gcIsMarked(params));
  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(lexEnv));
  EXPECT_T(gcIsMarked(clo));
}

void test_objMarkExn() {
  /* allocate some adjacent objects */
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object exn = exnNew(i200);

  EXPECT_F(gcIsMarked(i100));
  EXPECT_F(gcIsMarked(i200));
  EXPECT_F(gcIsMarked(i300));
  EXPECT_F(gcIsMarked(exn));

  objMark(exn);

  EXPECT_F(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
  EXPECT_F(gcIsMarked(i300));
  EXPECT_T(gcIsMarked(exn));
}

void test_objMarkHash() {
  Object hash = hashNew();
  Object x = identNew("x");
  Object y = identNew("y");
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  hashPut(hash, x, i100);
  hashPut(hash, y, i200);

  objMark(hash);

  EXPECT_T(gcIsMarked(hash));
  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(y));
  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
}

void test_objMarkList() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object list = listNew(i100, i200);

  objMark(list);

  EXPECT_T(gcIsMarked(list));
  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
}

void test_objMarkQueue() {
  Object q = queueNew();
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  queueEnq(q, i100);
  queueEnq(q, i200);
  queueEnq(q, i300);

  objMark(q);

  EXPECT_T(gcIsMarked(q));
  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
  EXPECT_T(gcIsMarked(i300));
}

void test_objMarkSet() {
  Object set = setNew();
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  setAddElem(set, i100);
  setAddElem(set, i200);
  setAddElem(set, i300);

  objMark(set);

  EXPECT_T(gcIsMarked(set));
  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(i200));
  EXPECT_T(gcIsMarked(i300));
}

void test_objMarkAbstr1() {
  Object x = identNew("x");
  Object params = listNew(x, EMPTY_LIST);
  Object body = intNew(100);
  Object abstr = abstrNew(params, body);

  objMark(abstr);

  EXPECT_T(gcIsMarked(abstr));
  EXPECT_T(gcIsMarked(params));
  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(body));
}

void test_objMarkAbstr2() {
  Object x = identNew("x");
  Object params1 = listNew(x, EMPTY_LIST);
  Object body1 = intNew(100);
  Object rule1 = abstrNew(params1, body1);

  Object y = identNew("y");
  Object params2 = listNew(y, EMPTY_LIST);
  Object body2 = intNew(200);
  Object rule2 = abstrNew(params2, body2);

  abstrSetNext(rule1, rule2);

  objMark(rule1);

  EXPECT_T(gcIsMarked(rule1));
  EXPECT_T(gcIsMarked(params1));
  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(body1));
  EXPECT_T(gcIsMarked(rule2));
  EXPECT_T(gcIsMarked(params2));
  EXPECT_T(gcIsMarked(y));
  EXPECT_T(gcIsMarked(body2));
}

void test_objMarkApp() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object app = appNew(x, i100);

  objMark(app);

  EXPECT_T(gcIsMarked(app));
  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(i100));
}

void test_objMarkIf() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object eIf = ifNew(x, y, z);

  objMark(eIf);

  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(y));
  EXPECT_T(gcIsMarked(z));
  EXPECT_T(gcIsMarked(eIf));
}

void test_objMarkLet() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object bnd = bindingNew(x, i100);
  Object bindings = listNew(bnd, EMPTY_LIST);
  Object let = letNew(bindings);

  objMark(let);

  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(bnd));
  EXPECT_T(gcIsMarked(bindings));
  EXPECT_T(gcIsMarked(let));
}

void test_objMarkLetIn() {
  Object x = identNew("x");
  Object i100 = intNew(100);
  Object bnd = bindingNew(x, i100);
  Object bindings = listNew(bnd, EMPTY_LIST);
  Object body = identNew("y");
  Object letIn = letInNew(bindings, body);

  objMark(letIn);

  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(i100));
  EXPECT_T(gcIsMarked(bnd));
  EXPECT_T(gcIsMarked(bindings));
  EXPECT_T(gcIsMarked(body));
  EXPECT_T(gcIsMarked(letIn));
}

void test_objMarkSeq() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object z = identNew("z");
  Object exprs = listNew(x, listNew(y, listNew(z, EMPTY_LIST)));
  Object seq = seqNew(exprs);

  objMark(seq);

  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(y));
  EXPECT_T(gcIsMarked(z));
  EXPECT_T(gcIsMarked(exprs));
  EXPECT_T(gcIsMarked(seq));
}

void test_objMarkThrow() {
  Object x = identNew("x");
  Object throw = throwNew(x);

  objMark(throw);

  EXPECT_T(gcIsMarked(x));
  EXPECT_T(gcIsMarked(throw));
}

void test_objCopy() {
  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object i300 = intNew(300);
  Object ary0 = arrayNew(3);
  arraySet(ary0, 0, i100);
  arraySet(ary0, 1, i200);
  arraySet(ary0, 2, i300);
  Object ary1 = objCopy(ary0);
  EXPECT_NE(ary0.a, ary1.a);
  EXPECT_T(objEquals(ary0, ary1));
  Object lst0 = listNew(i100, i200);
  Object lst1 = objCopy(lst0);
  EXPECT_NE(lst0.a, lst1.a);
  EXPECT_T(objEquals(lst0, lst1));
}
