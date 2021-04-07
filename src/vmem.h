#ifndef VMEM_H
#define VMEM_H

#include <stdio.h>

#include "defines.h"

/* The address range is 2^sizeof(uint). Note that memory is in words,
   not bytes. With a 16-bit address space that equates to 64KW or
   128KB of virtual memory.
*/
typedef uint Address;

/* Word is what is stored at a memory location. */
typedef uint Word;

/* The page size is the size in words of each page. */
#define PAGE_SIZE 1024
/* This is the number of in-memory pages. Thus, if Word is 16 bits,
   then 8 x 1024 pages = 16384 bytes. */
#define N_PAGES 8

/* Number of keys per page. For any given page in memory, this is how
   many pages on disk map to that one memory page. */
#define N_KEYS 8

#define KEYMASK(addr)    (addr / PAGE_SIZE / N_PAGES)
#define PAGEMASK(addr)   (addr / PAGE_SIZE % N_PAGES)
#define OFFSETMASK(addr) (addr % PAGE_SIZE)

FILE* vmemStart();
void vmemStop();

Word vmemGet(Address addr);
void vmemSet(Address addr, Word value);
long vmemGetNWords();

void vmemInc(Address addr);
void vmemDec(Address addr);

#endif
