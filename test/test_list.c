#include <stdio.h>

#include "test.h"

#include "../src/d_binding.h"
#include "../src/d_int.h"
#include "../src/d_list.h"
#include "../src/delegate.h"
#include "../src/e_ident.h"
#include "../src/globals.h"
#include "../src/mem.h"
#include "../src/object.h"

void test_listEmptyGet();
void test_listEmptySet();
void test_listEquals();
void test_listGet();
void test_listIsEmpty();
void test_listMatch();
void test_listSet();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_listEmptyGet", test_listEmptyGet},
  {"test_listEmptySet", test_listEmptySet},
  {"test_listGet", test_listGet},
  {"test_listSet", test_listSet},
  {"test_listIsEmpty", test_listIsEmpty},
  {"test_listEquals", test_listEquals},
  {"test_listMatch", test_listMatch},
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

void test_list() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_listEmptyGet() {
  EXPECT_NE(0, EMPTY_LIST.a);

  Object p1 = listGetFirst(EMPTY_LIST);
  EXPECT_EQ(NOTHING.a, p1.a);
  p1 = listGetRest(EMPTY_LIST);
  EXPECT_EQ(EMPTY_LIST.a, p1.a);
}

void test_listEmptySet() {
  Object obj = intNew(100);
  ASSERT_NE(0, obj.a);
  listSetFirst(EMPTY_LIST, obj);
  EXPECT_EQ(NOTHING.a, listGetFirst(EMPTY_LIST).a);

  listSetRest(EMPTY_LIST, obj);
  EXPECT_EQ(EMPTY_LIST.a, listGetRest(EMPTY_LIST).a);
}

void test_listEquals() {
  EXPECT_T(listEquals(EMPTY_LIST, EMPTY_LIST, thd));

  Object i100 = intNew(100);
  Object i200 = intNew(200);

  Object list1 = listNew(i100, EMPTY_LIST);
  EXPECT_T(listEquals(list1, list1, thd));

  Object list2 = listNew(i100, EMPTY_LIST);
  EXPECT_T(listEquals(list1, list2, thd));
  EXPECT_T(listEquals(list2, list1, thd));

  Object list3 = listNew(i200, EMPTY_LIST);
  EXPECT_F(listEquals(list1, list3, thd));
  EXPECT_F(listEquals(list2, list3, thd));
  EXPECT_F(listEquals(list3, list1, thd));
  EXPECT_F(listEquals(list3, list2, thd));
}

void test_listGet() {
  Object obj1 = intNew(100);
  Object obj2 = intNew(200);
  Object list = listNew(obj1, obj2);
  EXPECT_EQ(obj1.a, listGetFirst(list).a);
  EXPECT_EQ(obj2.a, listGetRest(list).a);
}

void test_listIsEmpty() {
  EXPECT_T(listIsEmpty(EMPTY_LIST));
  Object obj1 = intNew(100);
  Object obj2 = intNew(200);
  Object list = listNew(obj1, obj2);
  EXPECT_T(!listIsEmpty(list));
}


void test_listMatch() {
  Object x = identNew("x");
  Object y = identNew("y");
  Object lst1 = listNew(x, y);

  Object i100 = intNew(100);
  Object i200 = intNew(200);
  Object lst2 = listNew(i100, i200);

  Object bindingList = EMPTY_LIST;
  Object bindingList1 = objMatch(lst1, lst2, bindingList, thd);

  Object res = listLocate(bindingList1, x, thd);
  EXPECT_T(objEquals(bindingNew(x, i100), res, thd));
  res = listLocate(bindingList1, y, thd);
  EXPECT_T(objEquals(bindingNew(y, i200), res, thd));
}

void test_listSet() {
  Object obj1 = intNew(100);
  Object obj2 = intNew(200);
  Object list = listNew(obj1, obj2);

  Object obj3 = intNew(300);
  Object obj4 = intNew(400);
  listSetFirst(list, obj3);
  listSetRest(list, obj4);
  EXPECT_EQ(obj3.a, listGetFirst(list).a);
  EXPECT_EQ(obj4.a, listGetRest(list).a);
}
