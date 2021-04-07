#ifndef TEST_H
#define TEST_H

#include <assert.h>
#include <stdlib.h>

typedef struct {
  char* name;
  void (*test)();
} TestEntry;

typedef struct {
  int nPasses;
  int nFailures;
} PassFail;

extern PassFail __pf__;
extern PassFail __pfTestRun__;
extern PassFail __pfTotal__;

void runTests(char* name, void (*before)(), TestEntry* testEntries, void (*after)());

#define EXPECT_T( expr ) if (!(expr)) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n", __FILE__, __FUNCTION__, __LINE__); } else { __pf__.nPasses++; }

#define EXPECT_F( expr ) if (expr) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n", __FILE__, __FUNCTION__, __LINE__); } else { __pf__.nPasses++; }

#define ASSERT_EQ( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if (__lhsExpr != __rhs) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n    expected == %ld\n    found %ld\n", __FILE__, __FUNCTION__, __LINE__, __lhs, __rhs); exit(1); } else { __pf__.nPasses++; } }

#define ASSERT_NE( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if (__lhsExpr == __rhs) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n    expected != %ld\n    found %ld\n", __FILE__, __FUNCTION__, __LINE__, __lhs, __rhs); exit(1); } else { __pf__.nPasses++; } }

#define EXPECT_EQ( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if (__lhsExpr != __rhs) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n    expected == %ld\n    found %ld\n", __FILE__, __FUNCTION__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#define EXPECT_NE( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if (__lhsExpr == __rhs) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n    expected != %ld\n    found %ld\n", __FILE__, __FUNCTION__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#define EXPECT_LT( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if (__lhsExpr >= __rhs) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n    expected < %ld\n    found %ld\n", __FILE__, __FUNCTION__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#define EXPECT_GT( __lhsExpr, __rhsExpr ) { long __lhs = (long)__lhsExpr; long __rhs = (long)__rhsExpr; if (__lhsExpr <= __rhs) { __pf__.nFailures++; fprintf(stderr, "FAILURE: %s: %s line %d\n    expected > %ld\n    found %ld\n", __FILE__, __FUNCTION__, __LINE__, __lhs, __rhs); } else { __pf__.nPasses++; } }

#endif
