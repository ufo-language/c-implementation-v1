#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/d_queue.h"
#include "../src/d_real.h"
#include "../src/d_string.h"
#include "../src/d_symbol.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/lexer.h"
#include "../src/lexer_obj.h"
#include "../src/object.h"

static void test_lexInt1();
static void test_lexInt2();
static void test_lexReal();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_lexInt1", test_lexInt1},
  {"test_lexInt2", test_lexInt2},
  {"test_lexReal", test_lexReal},
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

void test_lexer() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_lexInt1() {
  char* input = "123";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object intTok = queueDeq(tokenQ);
  Object intTokSym = symbolNew("Int");
  Object intTokVal = intNew(123);
  ASSERT_NE(nullObj.a, intTok.a);
  EXPECT_T(objEqual(intTokSym, arrayGet(intTok, 0)));
  EXPECT_T(objEqual(intTokVal, arrayGet(intTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  Object eoiTokSym = symbolNew("EOI");
  ASSERT_NE(nullObj.a, eoiTok.a);
  EXPECT_T(objEqual(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexInt2() {
  char* input = "123 456";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(3, queueCount(tokenQ));

  Object intTok1 = queueDeq(tokenQ);
  Object intTokSym = symbolNew("Int");
  Object intTokVal1 = intNew(123);
  ASSERT_NE(nullObj.a, intTok1.a);
  EXPECT_T(objEqual(intTokSym, arrayGet(intTok1, 0)));
  EXPECT_T(objEqual(intTokVal1, arrayGet(intTok1, 1)));

  Object intTok2 = queueDeq(tokenQ);
  Object intTokVal2 = intNew(456);
  ASSERT_NE(nullObj.a, intTok2.a);
  EXPECT_T(objEqual(intTokSym, arrayGet(intTok2, 0)));
  EXPECT_T(objEqual(intTokVal2, arrayGet(intTok2, 1)));

  Object eoiTok = queueDeq(tokenQ);
  Object eoiTokSym = symbolNew("EOI");
  ASSERT_NE(nullObj.a, eoiTok.a);
  EXPECT_T(objEqual(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexReal() {
  char* input = "123.456";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  printf("test_lexReal tokens = "); objShow(tokenQ, stdout); printf("\n");
  ASSERT_EQ(2, queueCount(tokenQ));
  Object realTok = queueDeq(tokenQ);
  Object realTokSym = symbolNew("Real");
  Object realTokVal = realNew(123.456);
  ASSERT_NE(nullObj.a, realTok.a);
  EXPECT_T(objEqual(realTokSym, arrayGet(realTok, 0)));
  EXPECT_T(objEqual(realTokVal, arrayGet(realTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  Object eoiTokSym = symbolNew("EOI");
  ASSERT_NE(nullObj.a, eoiTok.a);
  EXPECT_T(objEqual(eoiTokSym, arrayGet(eoiTok, 0)));
}

