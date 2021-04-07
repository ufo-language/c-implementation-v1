#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "blockfile.h"
#include "vmem.h"

#define PAGE_FILE_NAME "pagefile.dat"

static FILE* pageFile = 0;

byte pageMap[N_PAGES];
Word pageTable[N_PAGES][PAGE_SIZE];
bool isDirty[N_PAGES];

/* Returns the total number of words provided by the virtual memory
   system. */
long vmemGetNWords() {
  return PAGE_SIZE * N_PAGES * N_KEYS;
}

FILE* vmemStart() {
  if (pageFile) {
    fclose(pageFile);
  }
  /* create and initialize the page file */
  pageFile = blockFileInit(PAGE_FILE_NAME, PAGE_SIZE * sizeof(Word), N_KEYS * N_PAGES);
  /* clear the page arrays in memory */
  for (int pageN=0; pageN<N_PAGES; pageN++) {
    pageMap[pageN] = 0;
    isDirty[pageN] = false;
    memset(pageTable[pageN], 0, PAGE_SIZE * sizeof(Word));
  }
  return pageFile;
}

void vmemStop() {
  fclose(pageFile);
}

/* Reads a page from the page file.
   pageNum = index of page/block in file
   pageKey = high bits of pageNum
   pageIndex = low bits of pageNum
*/
void pageFileRead(uint pageNum, byte pageKey, uint pageIndex) {
  byte* page = (byte*)pageTable[pageIndex];
  assert(pageFile != 0);
  blockFileRead(page, PAGE_SIZE * sizeof(uint), pageNum, pageFile);
  assert(pageFile != 0);
  pageMap[pageIndex] = pageKey;
}

/* Writes a page to the page file. */
void pageFileWrite(uint pageIndex) {
  uint pageFileIndex = pageMap[pageIndex] * N_PAGES + pageIndex;
  byte* page = (byte*)pageTable[pageIndex];
  blockFileWrite(page, PAGE_SIZE * sizeof(Word), pageFileIndex, pageFile);
}

/* Returns an in-memory page. If the actual requested page is not in
   memory, a page fault occurs and the page is loaded from the on-disk
   page file.*/
Word* pageGet(uint pageNum) {
  byte pageKey = pageNum / N_PAGES;
  uint pageIndex = pageNum % N_PAGES;
  if (pageMap[pageIndex] != pageKey) {
    /* page fault */
    pageFileWrite(pageIndex);
    pageFileRead(pageNum, pageKey, pageIndex);
  }
  return pageTable[pageIndex];
}

/* Gets a word from memory. The address must fall within the ramge of
   all of virtual memory.*/
Word vmemGet(Address addr) {
  Word* page = pageGet(addr / PAGE_SIZE);
  Word w = page[addr % PAGE_SIZE];
  return w;
}

/* Sets the value of a word in memory. The address must fall within
   the range of all of virtual memory. */
void vmemSet(Address addr, Word value) {
  if (addr == 0) {
    fprintf(stderr, "vmemSet ERROR: attempt to write to address 0, value = %d\n", value);
  }
  Word* page = pageGet(addr / PAGE_SIZE);
  page[addr % PAGE_SIZE] = value;
}

void vmemInc(Address addr) {
  Word* page = pageGet(addr / PAGE_SIZE);
  page[addr % PAGE_SIZE]++;
}

void vmemDec(Address addr) {
  Word* page = pageGet(addr / PAGE_SIZE);
  page[addr % PAGE_SIZE]--;
}
