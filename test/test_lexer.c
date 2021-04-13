#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_bool.h"
#include "../src/d_int.h"
#include "../src/d_queue.h"
#include "../src/d_real.h"
#include "../src/d_string.h"
#include "../src/d_symbol.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/globals.h"
#include "../src/lexer.h"
#include "../src/lexer_obj.h"
#include "../src/object.h"
#include "../src/syntax.h"

static void test_lexInt1();
static void test_lexInt2();
static void test_lexReal();
static void test_lexIdent();
static void test_lexReserved();
static void test_lexString();
static void test_lexOper();
static void test_lexSpecial();
static void test_lexBool();
static void test_lexMultiple();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_lexInt1", test_lexInt1},
  {"test_lexInt2", test_lexInt2},
  {"test_lexReal", test_lexReal},
  {"test_lexIdent", test_lexIdent},
  {"test_lexReserved", test_lexReserved},
  {"test_lexString", test_lexString},
  {"test_lexOper", test_lexOper},
  {"test_lexSpecial", test_lexSpecial},
  {"test_lexBool", test_lexBool},
  {"test_lexMultiple", test_lexMultiple},
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

// TODO debugging
extern Transition pointState[];
extern Transition realState[];
extern char* S_NAMES[];

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
  Object intTokSym = symbolNew(T_NAMES[T_INT]);
  Object intTokVal = intNew(123);
  ASSERT_NE(nullObj.a, intTok.a);
  EXPECT_T(objEquals(intTokSym, arrayGet(intTok, 0)));
  EXPECT_T(objEquals(intTokVal, arrayGet(intTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexInt2() {
  char* input = "123 456";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(3, queueCount(tokenQ));

  Object intTok1 = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, intTok1.a);
  Object intTokSym = symbolNew(T_NAMES[T_INT]);
  Object intTokVal1 = intNew(123);
  EXPECT_T(objEquals(intTokSym, arrayGet(intTok1, 0)));
  EXPECT_T(objEquals(intTokVal1, arrayGet(intTok1, 1)));

  Object intTok2 = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, intTok2.a);
  Object intTokVal2 = intNew(456);
  EXPECT_T(objEquals(intTokSym, arrayGet(intTok2, 0)));
  EXPECT_T(objEquals(intTokVal2, arrayGet(intTok2, 1)));

  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexReal() {
  char* input = "123.45";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object realTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, realTok.a);
  Object realTokSym = symbolNew(T_NAMES[T_REAL]);
  Object realTokVal = realNew(123.45);
  EXPECT_T(objEquals(realTokSym, arrayGet(realTok, 0)));
  EXPECT_T(objEquals(realTokVal, arrayGet(realTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexIdent() {
  char* input = "abc";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object identTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, identTok.a);
  Object identTokSym = symbolNew(T_NAMES[T_IDENT]);
  Object identTokVal = identNew(input);
  EXPECT_T(objEquals(identTokSym, arrayGet(identTok, 0)));
  EXPECT_T(objEquals(identTokVal, arrayGet(identTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexReserved() {
  char* input = "end";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object reservedTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, reservedTok.a);
  Object reservedTokSym = symbolNew(T_NAMES[T_RESERVED]);
  Object reservedTokVal = stringNew(input);
  EXPECT_T(objEquals(reservedTokSym, arrayGet(reservedTok, 0)));
  EXPECT_T(objEquals(reservedTokVal, arrayGet(reservedTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexString() {
  char* input = "\"hello, world!\"";
  char* tokenValue = "hello, world!";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object stringTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, stringTok.a);
  Object stringTokSym = symbolNew(T_NAMES[T_STRING]);
  Object stringTokVal = stringNew(tokenValue);
  EXPECT_T(objEquals(stringTokSym, arrayGet(stringTok, 0)));
  EXPECT_T(objEquals(stringTokVal, arrayGet(stringTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexOper() {
  char* input = "++";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object operTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, operTok.a);
  Object operTokSym = symbolNew(T_NAMES[T_OPER]);
  Object operTokVal = identNew(input);
  EXPECT_T(objEquals(operTokSym, arrayGet(operTok, 0)));
  EXPECT_T(objEquals(operTokVal, arrayGet(operTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexSpecial() {
  char* input = "(";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object specialTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, specialTok.a);
  Object specialTokSym = symbolNew(T_NAMES[T_SPECIAL]);
  Object specialTokVal = stringNew(input);
  EXPECT_T(objEquals(specialTokSym, arrayGet(specialTok, 0)));
  EXPECT_T(objEquals(specialTokVal, arrayGet(specialTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexBool() {
  char* input = "true";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object boolTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, boolTok.a);
  Object boolTokSym = symbolNew(T_NAMES[T_BOOL]);
  Object boolTokVal = boolNew(true);
  EXPECT_T(objEquals(boolTokSym, arrayGet(boolTok, 0)));
  EXPECT_T(objEquals(boolTokVal, arrayGet(boolTok, 1)));
  Object eoiTok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet(eoiTok, 0)));
}

void test_lexMultiple() {
  char* input = "123 45.6 abc end false \"Hello\" ; ++";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  ASSERT_EQ(9, queueCount(tokenQ));

  Object tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  Object tokSym = symbolNew(T_NAMES[T_INT]);
  Object tokVal = intNew(123);
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_REAL]);
  tokVal = realNew(45.6);
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_IDENT]);
  tokVal = identNew("abc");
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_RESERVED]);
  tokVal = stringNew("end");
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_BOOL]);
  tokVal = boolNew(false);
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_STRING]);
  tokVal = stringNew("Hello");
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_SPECIAL]);
  tokVal = stringNew(";");
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_OPER]);
  tokVal = identNew("++");
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
  EXPECT_T(objEquals(tokVal, arrayGet(tok, 1)));

  tok = queueDeq(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(tokSym, arrayGet(tok, 0)));
}
