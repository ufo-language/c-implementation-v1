#include <stdio.h>
#include <string.h>

#include "test.h"

#include "../src/d_string.h"
#include "../src/mem.h"
#include "../src/object.h"

void test_stringNew();
void test_stringEquals();
void test_stringBounds();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_stringNew", test_stringNew},
  {"test_stringEquals", test_stringEquals},
  {"test_stringBounds", test_stringBounds},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static void test_before() {
  memStart();
}

static void test_after() {
  memStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_string() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_stringNew() {
  char* str = "Abcd";
  Object string = stringNew(str);
  Word nChars = stringCount(string);
  EXPECT_EQ((int)strlen(str), nChars);
  for (Word n=0; n<nChars; n++) {
    EXPECT_EQ(str[n], stringGetChar_unsafe(string, n));
  }
}

void test_stringEquals() {
  char* s0 = "";
  Object str0 = stringNew(s0);
  EXPECT_T(stringEquals(str0, str0));
  
  char* s1 = "Abcd";
  Object str1 = stringNew(s1);
  EXPECT_T(stringEquals(str1, str1));
  EXPECT_F(stringEquals(str0, str1));
  EXPECT_F(stringEquals(str1, str0));

  Object str2 = stringNew(s1);
  EXPECT_T(stringEquals(str1, str2));
  EXPECT_T(stringEquals(str2, str1));

  char* s2 = "Defg";
  Object str3 = stringNew(s2);
  EXPECT_F(stringEquals(str1, str3));
  EXPECT_F(stringEquals(str2, str3));
  EXPECT_F(stringEquals(str3, str1));
}

void test_stringBounds() {
  char* s = "Abcd";
  Object str = stringNew(s);
  int nChars = strlen(s);
  for (int n=0; n<nChars; n++) {
    EXPECT_EQ(s[n], stringGetChar_unsafe(str, n));
  }
  EXPECT_EQ(0, stringGetChar_unsafe(str, nChars));
}
