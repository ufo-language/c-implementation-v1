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
static void test_parseIgnore();
static void test_parseSeqWithIgnore();
static void test_parseSome();
static void test_parseReserved();
static void test_parseIdent();
static void test_parseObject();
static void test_parseIf();

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
  {"test_parseIgnore", test_parseIgnore},
  {"test_parseSeqWithIgnore", test_parseSeqWithIgnore},
  {"test_parseSome", test_parseSome},
  {"test_parseReserved", test_parseReserved},
  {"test_parseIdent", test_parseIdent},
  {"test_parseObject", test_parseObject},
  {"test_parseIf", test_parseIf},
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
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_spot(tokens, T_SPECIAL);
  EXPECT_EQ(nullObj.a, res.a);
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
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse falure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_maybe(tokens, p_int);
  EXPECT_NE(nullObj.a, res.a);
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
  EXPECT_NE(nullObj.a, res.a);
  /* test another parse success */
  input = "\"abc\"";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_oneOf(tokens, parsers);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse falure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_oneOf(tokens, parsers);
  EXPECT_EQ(nullObj.a, res.a);
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
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseIgnore() {
  char* input = "100";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_ignore(tokens, p_int);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  EXPECT_EQ(NOTHING.a, resObj.a);
}

static Object p_ignoreInt(Object tokens) {
  return p_ignore(tokens, p_int);
}

void test_parseSeqWithIgnore() {
  char* input = "100 200 300";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Parser parsers[] = {p_ignoreInt, p_int, p_ignoreInt, NULL};
  Object res = p_seq(tokens, parsers);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  ASSERT_NE(NOTHING.a, resObj.a);
  Object resObjSym = arrayGet(resObj, 0);
  EXPECT_T(symbolHasName(resObjSym, T_NAMES[T_INT]));
  Object resObjVal = arrayGet(resObj, 1);
  EXPECT_EQ(200, intGet(resObjVal));
}

void test_parseReserved() {
  /* test a parse success */
  char* input = "end";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_spotReserved(tokens, "end");
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "100";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_spotReserved(tokens, "end");
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseIdent() {
  /* test a parse success */
  char* input = "abc";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_ident(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "100";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_ident(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseObject() {
  /* test a parse success */
  char* input = "100";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_object(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse success */
  input = "Abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse success */
  input = "\"hello world\"";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse success */
  input = "3.14159";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

Object p_IF(Object tokens);
Object p_THEN(Object tokens);
Object p_ELSE(Object tokens);
Object p_END(Object tokens);

void test_parseIf() {
  /* test IF reserved word */
  char* input = "if x then y else z end";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_IF(tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test THEN reserved word */
  input = "then y else z end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_THEN(tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test ELSE reserved word */
  input = "else z end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_ELSE(tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test END reserved word */
  input = "end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_END(tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test a parse success */
  input = "if x then y else z end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_if(tokens);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  EXPECT_EQ(E_If, objGetType(resObj));
  /* test a parse failure */
  input = "x";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_if(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}
