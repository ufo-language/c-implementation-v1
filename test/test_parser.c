#include <stdio.h>

#include "test.h"

#include "../src/d_list.h"
#include "../src/d_queue.h"
#include "../src/d_string.h"
#include "../src/delegate.h"
#include "../src/globals.h"
#include "../src/lexer_obj.h"
#include "../src/object.h"
#include "../src/parser.h"

static void test_parseSpot();
static void test_parseInt();
static void test_parseReal();
static void test_parseMaybe();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_parseSpot", test_parseSpot},
  {"test_parseInt", test_parseInt},
  {"test_parseReal", test_parseReal},
  //{"test_parseMaybe", test_parseMaybe},
  {0, }
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
  char* input = "";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_spot(tokens, T_EOI, nullObj);
  ASSERT_NE(nullObj.a, res.a);

  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_spot(tokens, T_SPECIAL, nullObj);
  ASSERT_EQ(nullObj.a, res.a);
}

void test_parseInt() {
  char* input = "123";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_int(tokens);
  EXPECT_NE(nullObj.a, res.a);

  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_int(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

void test_parseReal() {
  char* input = "123.45";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = p_real(tokens);
  printf("test_parseReal res = "); objShow(res, stdout); printf("\n");
  EXPECT_NE(nullObj.a, res.a);

  input = "abc";
  inputStr = stringNew(input);
  tokenQ = lex(inputStr);
  tokens = queueAsList(tokenQ);
  res = p_real(tokens);
  EXPECT_EQ(nullObj.a, res.a);
}

//Object p_real(Object tokens);
//Object p_number(Object tokens);
//Object p_string(Object tokens);

void test_parseMaybe() {
  char* input = "";
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  printf("tokenQ = "); objShow(tokenQ, stdout); printf("\n");
  /*
  Object res = p_maybe(tokens, *p_int);
  ASSERT_NE(nullObj.a, res.a);
*/
}
