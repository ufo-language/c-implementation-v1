#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blockfile.h"
#include "stacktrace.h"
#include "vmem.h"

#define PAGE_FILE_NAME "pagefile.dat"

static FILE* pageFile = 0;

/* This is a map from in-memory page (the array index) to on-disk page
   (the array value). */
byte vmemPageMap[N_PAGES];

/* These are the actual in-memory pages. */
Word vmemPageTable[N_PAGES][PAGE_SIZE];

bool vmemIsDirty[N_PAGES];

/*------------------------------------------------------------------*/
/* Returns the total number of words provided by the virtual memory
   system. */
long vmemGetNWords(void) {
  return PAGE_SIZE * N_PAGES * N_KEYS;
}

/*------------------------------------------------------------------*/
FILE* vmemStart(void) {
  if (pageFile) {
    fclose(pageFile);
  }
  /* create and initialize the page file */
  pageFile = blockFileInit(PAGE_FILE_NAME, PAGE_SIZE * sizeof(Word), N_KEYS * N_PAGES);
  /* clear the page arrays in memory */
  for (int pageN=0; pageN<N_PAGES; pageN++) {
    vmemPageMap[pageN] = 0;
    vmemIsDirty[pageN] = false;
    memset(vmemPageTable[pageN], 0, PAGE_SIZE * sizeof(Word));
  }
  return pageFile;
}

/*------------------------------------------------------------------*/
void vmemStop(void) {
  fclose(pageFile);
}

/*------------------------------------------------------------------*/
/* Reads a page from the page file.
   pageNum = index of page/block in file
   pageKey = high bits of pageNum
   pageIndex = low bits of pageNum
*/
void pageFileRead(uint pageNum, byte pageKey, uint pageIndex) {
  byte* page = (byte*)vmemPageTable[pageIndex];
  assert(pageFile != 0);
  blockFileRead(page, PAGE_SIZE * sizeof(uint), pageNum, pageFile);
  assert(pageFile != 0);
  vmemPageMap[pageIndex] = pageKey;
}

/*------------------------------------------------------------------*/
/* Writes a page to the page file. */
void pageFileWrite(uint pageIndex) {
  uint pageFileIndex = vmemPageMap[pageIndex] * N_PAGES + pageIndex;
  byte* page = (byte*)vmemPageTable[pageIndex];
  blockFileWrite(page, PAGE_SIZE * sizeof(Word), pageFileIndex, pageFile);
}

/*------------------------------------------------------------------*/
/* Returns an in-memory page. If the actual requested page is not in
   memory, a page fault occurs and the page is loaded from the on-disk
   page file.*/
uint pageGet(uint pageNum) {
  byte pageKey = pageNum / N_PAGES;
  uint pageIndex = pageNum % N_PAGES;
  if (vmemPageMap[pageIndex] != pageKey) {
    /* page fault */
    if (vmemIsDirty[pageIndex]) {
      pageFileWrite(pageIndex);
    }
    pageFileRead(pageNum, pageKey, pageIndex);
    vmemIsDirty[pageIndex] = false;
  }
  return pageIndex;
}

/*------------------------------------------------------------------*/
/* Gets a word from memory. The address must fall within the ramge of
   all of virtual memory.*/
Word vmemGet(Address addr) {
  uint pageIndex = pageGet(addr / PAGE_SIZE);
  Word* page = vmemPageTable[pageIndex];
  Word w = page[addr % PAGE_SIZE];
  return w;
}

/*------------------------------------------------------------------*/
/* Sets the value of a word in memory. The address must fall within
   the range of all of virtual memory. */
void vmemSet(Address addr, Word value) {
  if (addr == 0) {  /* TODO remove this after debugging? */
    fprintf(stderr, "vmemSet ERROR: attempt to write to address 0, value = %d\n", value);
    stackTrace();
    exit(1);
  }
  uint pageIndex = pageGet(addr / PAGE_SIZE);
  vmemIsDirty[pageIndex] = true;
  Word* page = vmemPageTable[pageIndex];
  page[addr % PAGE_SIZE] = value;
}

/*------------------------------------------------------------------*/
Word vmemInc(Address addr) {
  uint pageIndex = pageGet(addr / PAGE_SIZE);
  Word* page = vmemPageTable[pageIndex];
  return ++page[addr % PAGE_SIZE];
}

/*------------------------------------------------------------------*/
Word vmemIncBy(Address addr, Word w) {
  uint pageIndex = pageGet(addr / PAGE_SIZE);
  Word* page = vmemPageTable[pageIndex];
  return (page[addr % PAGE_SIZE] += w);
}

/*------------------------------------------------------------------*/
Word vmemDec(Address addr) {
  uint pageIndex = pageGet(addr / PAGE_SIZE);
  Word* page = vmemPageTable[pageIndex];
  return --page[addr % PAGE_SIZE];
}

/*------------------------------------------------------------------*/
Word vmemDecBy(Address addr, Word w) {
  uint pageIndex = pageGet(addr / PAGE_SIZE);
  Word* page = vmemPageTable[pageIndex];
  return (page[addr % PAGE_SIZE] -= w);
}
