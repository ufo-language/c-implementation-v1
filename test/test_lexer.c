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
#include "../src/thread.h"

static void test_lexInt1();
static void test_lexInt2();
static void test_lexReal();
static void test_lexIdent();
static void test_lexSymbol();
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
  {"test_lexSymbol", test_lexSymbol},
  {"test_lexReserved", test_lexReserved},
  {"test_lexString", test_lexString},
  {"test_lexOper", test_lexOper},
  {"test_lexSpecial", test_lexSpecial},
  {"test_lexBool", test_lexBool},
  {"test_lexMultiple", test_lexMultiple},
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
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object intTok = queueDeq_unsafe(tokenQ);
  Object intTokSym = symbolNew(T_NAMES[T_INT]);
  Object intTokVal = intNew(123);
  ASSERT_NE(nullObj.a, intTok.a);
  EXPECT_T(objEquals(intTokSym, arrayGet_unsafe(intTok, 0), thd));
  EXPECT_T(objEquals(intTokVal, arrayGet_unsafe(intTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexInt2() {
  char* input = "123 456";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(3, queueCount(tokenQ));

  Object intTok1 = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, intTok1.a);
  Object intTokSym = symbolNew(T_NAMES[T_INT]);
  Object intTokVal1 = intNew(123);
  EXPECT_T(objEquals(intTokSym, arrayGet_unsafe(intTok1, 0), thd));
  EXPECT_T(objEquals(intTokVal1, arrayGet_unsafe(intTok1, 1), thd));

  Object intTok2 = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, intTok2.a);
  Object intTokVal2 = intNew(456);
  EXPECT_T(objEquals(intTokSym, arrayGet_unsafe(intTok2, 0), thd));
  EXPECT_T(objEquals(intTokVal2, arrayGet_unsafe(intTok2, 1), thd));

  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexReal() {
  char* input = "123.45";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object realTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, realTok.a);
  Object realTokSym = symbolNew(T_NAMES[T_REAL]);
  Object realTokVal = realNew(123.45);
  EXPECT_T(objEquals(realTokSym, arrayGet_unsafe(realTok, 0), thd));
  EXPECT_T(objEquals(realTokVal, arrayGet_unsafe(realTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexIdent() {
  char* input = "abc";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object identTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, identTok.a);
  Object identTokSym = symbolNew(T_NAMES[T_IDENT]);
  Object identTokVal = identNew(input);
  EXPECT_T(objEquals(identTokSym, arrayGet_unsafe(identTok, 0), thd));
  EXPECT_T(objEquals(identTokVal, arrayGet_unsafe(identTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexSymbol() {
  char* input = "Abc";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object symTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, symTok.a);
  Object symTokSym = symbolNew(T_NAMES[T_SYMBOL]);
  Object symTokVal = symbolNew(input);
  EXPECT_T(objEquals(symTokSym, arrayGet_unsafe(symTok, 0), thd));
  EXPECT_T(objEquals(symTokVal, arrayGet_unsafe(symTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexReserved() {
  char* input = "end";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object reservedTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, reservedTok.a);
  Object reservedTokSym = symbolNew(T_NAMES[T_RESERVED]);
  Object reservedTokVal = stringNew(input);
  EXPECT_T(objEquals(reservedTokSym, arrayGet_unsafe(reservedTok, 0), thd));
  EXPECT_T(objEquals(reservedTokVal, arrayGet_unsafe(reservedTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexString() {
  char* input = "\"hello, world!\"";
  char* tokenValue = "hello, world!";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object stringTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, stringTok.a);
  Object stringTokSym = symbolNew(T_NAMES[T_STRING]);
  Object stringTokVal = stringNew(tokenValue);
  EXPECT_T(objEquals(stringTokSym, arrayGet_unsafe(stringTok, 0), thd));
  EXPECT_T(objEquals(stringTokVal, arrayGet_unsafe(stringTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexOper() {
  char* input = "++";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object operTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, operTok.a);
  Object operTokSym = symbolNew(T_NAMES[T_OPER]);
  Object operTokVal = identNew(input);
  EXPECT_T(objEquals(operTokSym, arrayGet_unsafe(operTok, 0), thd));
  EXPECT_T(objEquals(operTokVal, arrayGet_unsafe(operTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexSpecial() {
  char* input = "(";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object specialTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, specialTok.a);
  Object specialTokSym = symbolNew(T_NAMES[T_SPECIAL]);
  Object specialTokVal = stringNew(input);
  EXPECT_T(objEquals(specialTokSym, arrayGet_unsafe(specialTok, 0), thd));
  EXPECT_T(objEquals(specialTokVal, arrayGet_unsafe(specialTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexBool() {
  char* input = "true";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(2, queueCount(tokenQ));
  Object boolTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, boolTok.a);
  Object boolTokSym = symbolNew(T_NAMES[T_BOOL]);
  Object boolTokVal = boolNew(true);
  EXPECT_T(objEquals(boolTokSym, arrayGet_unsafe(boolTok, 0), thd));
  EXPECT_T(objEquals(boolTokVal, arrayGet_unsafe(boolTok, 1), thd));
  Object eoiTok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, eoiTok.a);
  Object eoiTokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(eoiTokSym, arrayGet_unsafe(eoiTok, 0), thd));
}

void test_lexMultiple() {
  char* input = "123 45.6 abc end false \"Hello\" ; ++";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(thd, inputStr);
  ASSERT_EQ(9, queueCount(tokenQ));

  Object tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  Object tokSym = symbolNew(T_NAMES[T_INT]);
  Object tokVal = intNew(123);
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_REAL]);
  tokVal = realNew(45.6);
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_IDENT]);
  tokVal = identNew("abc");
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_RESERVED]);
  tokVal = stringNew("end");
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_BOOL]);
  tokVal = boolNew(false);
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_STRING]);
  tokVal = stringNew("Hello");
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_SPECIAL]);
  tokVal = stringNew(";");
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_OPER]);
  tokVal = identNew("++");
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
  EXPECT_T(objEquals(tokVal, arrayGet_unsafe(tok, 1), thd));

  tok = queueDeq_unsafe(tokenQ);
  ASSERT_NE(nullObj.a, tok.a);
  tokSym = symbolNew(T_NAMES[T_EOI]);
  EXPECT_T(objEquals(tokSym, arrayGet_unsafe(tok, 0), thd));
}
