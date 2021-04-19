#include <stdio.h>

#include "test.h"

#include "../src/d_array.h"
#include "../src/d_binding.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/d_queue.h"
#include "../src/d_string.h"
#include "../src/d_symbol.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/e_let.h"
#include "../src/globals.h"
#include "../src/lexer_obj.h"
#include "../src/object.h"
#include "../src/parser.h"

Object p_comma(Thread* thd, Object tokens);
Object p_let(Thread* thd, Object tokens);
Object p_sepBy(Thread* thd, Object tokens, Parser parser, Parser separator);

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
static void test_parseSepBy();
static void test_parseLet();

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
  {"test_parseSepBy", test_parseSepBy},
  {"test_parseLet", test_parseLet},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

Thread* _thd;

static void test_before() {
  memStart();
  globalsSetup();
  _thd = threadNew();
}

static void test_after() {
  threadDelete(_thd);
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
  Object res = p_bool(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse 'false' success */
  input = "false";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_bool(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failures */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_bool(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseInt() {
  /* test a parse success */
  char* input = "123";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_int(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failures */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_int(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseReal() {
  /* test a parse success */
  char* input = "123.45";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_real(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_real(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseString() {
  /* test a parse success */
  char* input = "\"abc\"";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_string(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_string(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseSymbol() {
  /* test a parse success */
  char* input = "Abc";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_symbol(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_symbol(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseMaybe() {
  /* test a parse success */
  char* input = "123";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_maybe(_thd, tokens, p_int);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse falure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_maybe(_thd, tokens, p_int);
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
  Object res = p_oneOf(_thd, tokens, parsers);
  EXPECT_NE(nullObj.a, res.a);
  /* test another parse success */
  input = "\"abc\"";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_oneOf(_thd, tokens, parsers);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse falure */
  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_oneOf(_thd, tokens, parsers);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseSome() {
  /* test a parse success */
  char* input = "100 200 300";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_some(_thd, tokens, p_int, 1);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  Object exp = listNew(intNew(100), listNew(intNew(200), listNew(intNew(300), EMPTY_LIST)));
  ASSERT_T(objEquals(exp, resObj));
  /* test a parse failure */
  input = "1 2 x";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_some(_thd, tokens, p_int, 3);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseSeq() {
  /* test a parse success */
  char* input = "100 \"abc\"";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Parser parsers[] = {p_int, p_string, NULL};
  Object res = p_seq(_thd, tokens, parsers);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  ASSERT_EQ(D_List, objGetType(resObj));
  Object obj = listGetFirst(resObj);
  Object exp = intNew(100);
  EXPECT_T(objEquals(exp, obj));
  obj = listGetFirst(listGetRest(resObj));
  exp = stringNew("abc");
  EXPECT_T(objEquals(exp, obj));
  /* test a parse failure */
  input = "100 200 \"abc\"";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_seq(_thd, tokens, parsers);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseIgnore() {
  char* input = "100";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_ignore(_thd, tokens, p_int);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  EXPECT_EQ(NOTHING.a, resObj.a);
}

static Object p_ignoreInt(Thread* thd, Object tokens) {
  return p_ignore(thd, tokens, p_int);
}

void test_parseSeqWithIgnore() {
  char* input = "100 200 300";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Parser parsers[] = {p_ignoreInt, p_int, p_ignoreInt, NULL};
  Object res = p_seq(_thd, tokens, parsers);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  EXPECT_T(objEquals(intNew(200), resObj));
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
  Object res = p_ident(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "100";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_ident(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseObject() {
  /* test a parse success */
  char* input = "100";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_object(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse success */
  input = "Abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse success */
  input = "\"hello world\"";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse success */
  input = "3.14159";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);
  /* test a parse failure */
  input = "end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_object(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

Object p_IF(Thread* thd, Object tokens);
Object p_THEN(Thread* thd, Object tokens);
Object p_ELSE(Thread* thd, Object tokens);
Object p_END(Thread* thd, Object tokens);

void test_parseIf() {
  /* test IF reserved word */
  char* input = "if x then y else z end";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_IF(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test THEN reserved word */
  input = "then y else z end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_THEN(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test ELSE reserved word */
  input = "else z end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_ELSE(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test END reserved word */
  input = "end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_END(_thd, tokens);
  EXPECT_NE(nullObj.a, res.a);

  /* test a parse success */
  input = "if true then 100 else 200 end";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_if(_thd, tokens);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  EXPECT_EQ(E_If, objGetType(resObj));
  /* test a parse failure */
  input = "x";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_if(_thd, tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseSepBy() {
  /* test parse success, multiple elements */
  char* input = "100, 200, 300";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_sepBy(_thd, tokens, p_object, p_comma);
  ASSERT_NE(nullObj.a, res.a);
  Object exp = listNew(intNew(100), listNew(intNew(200), listNew(intNew(300), EMPTY_LIST)));
  Object resObj = listGetFirst(res);
  EXPECT_T(objEquals(exp, resObj));
  /* test parse success, single element */
  input = "100";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_sepBy(_thd, tokens, p_object, p_comma);
  ASSERT_NE(nullObj.a, res.a);
  exp = listNew(intNew(100), EMPTY_LIST);
  resObj = listGetFirst(res);
  EXPECT_T(objEquals(exp, resObj));
  /* test parse success, no elements */
  input = "";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_sepBy(_thd, tokens, p_object, p_comma);
  ASSERT_NE(nullObj.a, res.a);
  exp = EMPTY_LIST;
  resObj = listGetFirst(res);
  EXPECT_T(objEquals(exp, resObj));
  /* parse failure not tested because it throws an exception, and I
     don't yet know how to handle that here */
}

void test_parseLet() {
  /* test parse success, one binding */
  char* input = "let a=100";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_let(_thd, tokens);
  ASSERT_NE(nullObj.a, res.a);
  Object resObj = listGetFirst(res);
  ASSERT_EQ(E_Let, objGetType(resObj));
  /* test parse success, two bindings */
  input = "let a=100, b=200";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_let(_thd, tokens);
   ASSERT_NE(nullObj.a, res.a);
  resObj = listGetFirst(res);
  ASSERT_EQ(E_Let, objGetType(resObj));
}
