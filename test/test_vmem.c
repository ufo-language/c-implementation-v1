#include <stdio.h>

#include "test.h"

#include "../src/globals.h"
#include "../src/vmem.h"

static void test_vmemStart();
static void test_vmem_offsets();
static void test_vmem_get();
static void test_vmem_set1();
static void test_vmem_set2();

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_vmemStart", test_vmemStart},
  {"test_vmem_offsets", test_vmem_offsets},
  {"test_vmem_get", test_vmem_get},
  {"test_vmem_set1", test_vmem_set1},
  {"test_vmem_set2", test_vmem_set2},
  {0, 0}
};

/* Before & after --------------------------------------------------*/

static void test_before() {
  vmemStart();
}

static void test_after() {
  vmemStop();
}

/* Runs all the listed tests ---------------------------------------*/

void test_vmem() {
  runTests((char*)__func__, test_before, testEntries, test_after);
}

/* Unit tests ------------------------------------------------------*/

void test_vmemStart() {
  /* Not sure what to test. It creates a file, and the file size is different
     on each different CPU bit-width architecture. */
}

void test_vmem_offsets() {
#if 0
  uint addr = 0b10111100000;
  byte key = KEYMASK(addr);
  uint page = PAGEMASK(addr);
  uint offset = OFFSETMASK(addr);
  EXPECT(addr == 1504);
  EXPECT(key == 2);
  EXPECT(page == 0xF);
  EXPECT(offset == 0);

  addr = 0b01000011111;
  key = KEYMASK(addr);
  page = PAGEMASK(addr);
  offset = OFFSETMASK(addr);
  EXPECT(addr == 543);
  EXPECT(key == 1);
  EXPECT(page == 0);
  EXPECT(offset == 0x1F);
#endif
}

void test_vmem_get() {
  long size = vmemGetNWords();
  for (long n=0; n<size; n++) {
    uint x = vmemGet((uint)n);
    EXPECT_EQ(0, x);
  }
}

void test_vmem_set1() {
  uint addr, val;

  addr = 1;
  val = 0x1234;
  vmemSet(addr, val);
  EXPECT_EQ(val, vmemGet(addr));
  vmemSet(addr, 0);
  EXPECT_EQ(0, vmemGet(addr));
  vmemSet(addr, 0x1234);

  addr = PAGE_SIZE;
  vmemSet(addr, val);
  EXPECT_EQ(val, vmemGet(addr));
  vmemSet(addr, 0);
  EXPECT_EQ(0, vmemGet(addr));

  addr = PAGE_SIZE * N_PAGES;
  val = 0x9876;
  vmemSet(addr, val);
  EXPECT_EQ(val, vmemGet(addr));

  addr = 1;
  EXPECT_EQ(0x1234, vmemGet(addr));
}

void test_vmem_set2() {
  long size = vmemGetNWords();
  for (long laddr=1; laddr<size; laddr++) {
    uint addr = (uint)laddr;
    vmemSet(addr, addr);
  }

  for (long laddr=1; laddr<size; laddr++) {
    uint addr = (uint)laddr;
    uint val = vmemGet(addr);
    EXPECT_EQ(addr, val);
  }
}
