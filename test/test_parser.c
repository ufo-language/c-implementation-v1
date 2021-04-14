#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_queue.h"
#include "../src/d_string.h"
#include "../src/d_symbol.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/lexer_obj.h"
#include "../src/object.h"
#include "../src/parser.h"

static void test_parseSpot();
static void test_parseBool();
static void test_parseInt();
static void test_parseReal();
static void test_parseString();
static void test_parseSymbol();
static void test_parseMaybe();
static void test_parseOneOf();
static void test_parseSeq();
static void test_parseSome();
static void test_parseReserved();
static void test_parseIdent();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_parseSpot", test_parseSpot},
  {"test_parseBool", test_parseBool},
  {"test_parseInt", test_parseInt},
  {"test_parseReal", test_parseReal},
  {"test_parseString", test_parseString},
  {"test_parseSymbol", test_parseSymbol},
  {"test_parseMaybe", test_parseMaybe},
  {"test_parseOneOf", test_parseOneOf},
  {"test_parseSeq", test_parseSeq},
  {"test_parseSome", test_parseSome},
  {"test_parseReserved", test_parseReserved},
  {"test_parseIdent", test_parseIdent},
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

void test_parser() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_parseSpot() {
  /* test a parse success */
  char* input = "";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_spot(tokens, T_EOI);
  ASSERT_NE(nullObj.a, res.a);
  /* test a parse failure */
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_spot(tokens, T_SPECIAL);
  ASSERT_EQ(nullObj.a, res.a);
}

void test_parseBool() {
  /* test a parse 'true' success */
  char* input = "true";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_bool(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse 'false' success */
  input = "false";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_bool(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failures */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_bool(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseInt() {
  /* test a parse success */
  char* input = "123";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_int(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failures */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_int(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseReal() {
  /* test a parse success */
  char* input = "123.45";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_real(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_real(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseString() {
  /* test a parse success */
  char* input = "\"abc\"";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_string(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_string(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseSymbol() {
  /* test a parse success */
  char* input = "Abc";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_symbol(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_symbol(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseMaybe() {
  /* test a parse success */
  char* input = "123";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_maybe(tokens, p_int);
  ASSERT_NE(nullObj.a, res.a);
  /* test a parse falure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_maybe(tokens, p_int);
  ASSERT_NE(nullObj.a, res.a);
  Object token = listGetFirst(res);
  EXPECT_T(objEquals(NOTHING, token));
}

void test_parseOneOf() {
  /* test a parse success */
  char* input = "123";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Parser parsers[] = {p_int, p_string, NULL};
  Object res = p_oneOf(tokens, parsers);
  ASSERT_NE(nullObj.a, res.a);
  /* test another parse success */
  input = "\"abc\"";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_oneOf(tokens, parsers);
  ASSERT_NE(nullObj.a, res.a);
  /* test a parse falure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_oneOf(tokens, parsers);
  ASSERT_EQ(nullObj.a, res.a);
}

void test_parseSome() {
  /* test a parse success */
  char* input = "100 200 300";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_some(tokens, p_int, 1);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  ASSERT_EQ(D_List, objGetType(resObj));
  ASSERT_EQ(3, listCount(resObj));
  Object obj = listGetFirst(resObj);
  Object exp = arrayNew(2);
  arraySet(exp, 0, symbolNew("Int"));
  arraySet(exp, 1, intNew(100));
  EXPECT_T(objEquals(exp, obj));
  obj = listGetFirst(listGetRest(resObj));
  arraySet(exp, 0, symbolNew("Int"));
  arraySet(exp, 1, intNew(200));
  EXPECT_T(objEquals(exp, obj));
  obj = listGetFirst(listGetRest(listGetRest(resObj)));
  arraySet(exp, 0, symbolNew("Int"));
  arraySet(exp, 1, intNew(300));
  EXPECT_T(objEquals(exp, obj));
  /* test a parse failure */
  input = "1 2 x";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_some(tokens, p_int, 3);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseSeq() {
  /* test a parse success */
  char* input = "100 \"abc\"";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Parser parsers[] = {p_int, p_string, NULL};
  Object res = p_seq(tokens, parsers);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  ASSERT_EQ(D_List, objGetType(resObj));
  Object obj = listGetFirst(resObj);
  Object exp = arrayNew(2);
  arraySet(exp, 0, symbolNew("Int"));
  arraySet(exp, 1, intNew(100));
  EXPECT_T(objEquals(exp, obj));
  obj = listGetFirst(listGetRest(resObj));
  arraySet(exp, 0, symbolNew("String"));
  arraySet(exp, 1, stringNew("abc"));
  EXPECT_T(objEquals(exp, obj));
  /* test a parse failure */
  input = "100 200 \"abc\"";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_seq(tokens, parsers);
  ASSERT_EQ(nullObj.a, res.a);
}

void test_parseReserved() {
  /* test a parse success */
  char* input = "end";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_spotReserved(tokens, "end");
  ASSERT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "100";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_spotReserved(tokens, "end");
  ASSERT_EQ(nullObj.a, res.a);
}

void test_parseIdent() {
  /* test a parse success */
  char* input = "abc";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_ident(tokens);
  ASSERT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "100";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_ident(tokens);
  ASSERT_EQ(nullObj.a, res.a);
}
