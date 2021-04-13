#include <stdio.h>

#include "../src/gc.h"
#include "test.h"

int test_abstr();
int test_app();
int test_array();
int test_binding();
int test_blockFile();
int test_closure();
int test_delegate();
int test_eval();
int test_gc();
int test_hash();
int test_if();
int test_let();
int test_letIn();
int test_lexer();
int test_list();
int test_mem();
int test_object();
int test_parser();
int test_queue();
int test_seq();
int test_set();
int test_string();
int test_thread();
int test_throw();
int test_tuple();
int test_vmem();

static int (*tests[])() = {
  test_blockFile,
  test_vmem,
  test_mem,
  test_object,
  test_string,
  test_set,
  test_queue,
  test_eval,
  test_if,
  test_seq,
  test_throw,
  test_thread,
  test_hash,
  test_let,
  test_letIn,
  test_array,
  test_binding,
  test_list,
  test_closure,
  test_app,
  test_abstr,
  test_gc,
  test_lexer,
  test_parser,
  test_delegate,
  test_tuple,
  0
};

void test() {
  fprintf(stderr, "/ %s started\n\n", __func__);
  int nFailures = 0;

  for (int n=0; *tests[n]; n++) {
    __pfTestRun__.nPasses = 0;
    __pfTestRun__.nFailures = 0;
    tests[n]();
    __pfTotal__.nPasses += __pfTestRun__.nPasses;
    __pfTotal__.nFailures += __pfTestRun__.nFailures;
    fprintf(stderr, "\n");
  }

  fprintf(stderr, "\\ %s tests passed: %d/%d (%d failures)\n", __func__, __pfTotal__.nPasses, __pfTotal__.nPasses + __pfTotal__.nFailures, __pfTotal__.nFailures);
}

PassFail __pf__;
PassFail __pfTestRun__;
PassFail __pfTotal__;

void runTests(char* name, void (*before)(), TestEntry* testEntries, void (*after)()) {
  printf("/ %s started\n", name);

  while (1) {
    TestEntry testEntry = *testEntries;
    char* name = testEntry.name;
    if (!name) {
      break;
    }
    void (*test)() = testEntry.test;
    printf("| / %s started\n", name);
    __pf__.nPasses = 0;
    __pf__.nFailures = 0;
    before();
    test();
    after();
    gcCommit();
    printf("| \\ %s tests passed: %d/%d\n", name, __pf__.nPasses, __pf__.nPasses + __pf__.nFailures);
    __pfTestRun__.nPasses += __pf__.nPasses;
    __pfTestRun__.nFailures += __pf__.nFailures;
    testEntries++;
  }
  
  printf("\\ %s tests passed: %d/%d\n", name, __pfTestRun__.nPasses, __pfTestRun__.nPasses + __pfTestRun__.nFailures);
}
