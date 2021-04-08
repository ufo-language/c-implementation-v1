#include <stdio.h>

#include "test.h"

#include "../src/globals.h"
#include "../src/vmem.h"

static void test_vmemStart();
static void test_vmemOffsets();
static void test_vmemGet();
static void test_vmemSet1();
static void test_vmemSet2();
static void test_vmemDirty();

extern bool vmemIsDirty[N_PAGES];

/* List the unit tests to run here ---------------------------------*/

static TestEntry testEntries[] = {
  {"test_vmemStart", test_vmemStart},
  {"test_vmemOffsets", test_vmemOffsets},
  {"test_vmemGet", test_vmemGet},
  {"test_vmemSet1", test_vmemSet1},
  {"test_vmemSet2", test_vmemSet2},
  {"test_vmemDirty", test_vmemDirty},
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

void test_vmemOffsets() {
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

void test_vmemGet() {
  long size = vmemGetNWords();
  for (long n=0; n<size; n++) {
    uint x = vmemGet((uint)n);
    EXPECT_EQ(0, x);
  }
}

void test_vmemSet1() {
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

void test_vmemSet2() {
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

void test_vmemDirty() {
  for (int n=0; n<N_PAGES; n++) {
    EXPECT_F(vmemIsDirty[n]);
  }
  /* set the first word of vmem (well, skipping location 0) */
  vmemSet(1, 1);
  EXPECT_T(vmemIsDirty[0]);
  /* get a word from another page that maps to the same in-memory page */
  vmemGet(PAGE_SIZE * N_PAGES);
  EXPECT_F(vmemIsDirty[0]);
  /* get yet another page; this one should not cause a write-back */
  vmemGet(PAGE_SIZE * N_PAGES * 2);
  EXPECT_F(vmemIsDirty[0]);
  /* (I'm not sure what the best way is to test for a
     write-back. During initial testing I just printed a message when a
     write-back occurred. It worked fine) */
}
