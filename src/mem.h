#ifndef MEM_H
#define MEM_H

#include "defines.h"
#include "vmem.h"

#define MAX_ADDRESS 65535

#define MEMBLK_SIZE_OFFSET 0
#define MEMBLK_NEXT_OFFSET 1
#define MEMBLK_PREV_OFFSET 2
#define MEMBLK_DATA_OFFSET 2

/* Each free block has 3 words in addition to the data words. */
#define MEMBLK_OVERHEAD 3

/* When a block is in the free list, then the status word indicates
   the block's size (the size is stored both at the beginning of the
   block and also in the status word at the end of the block, in order
   to make it easy to manage block coalescing in a doubly-linked list
   of free blocks). When the block is in use, then the status word is
   set to 0 normally, but the GCer marks an object by setting the
   status to 32768. This means that in order for the `marked` flag to
   have meaning, the size of a block can't exceed 32767.
*/
#define MEMBLK_GC_MARK ((MAX_ADDRESS >> 1) + 1)

//typedef Address RawBlock;
//typedef Address Block;

typedef struct {
  Address a;
} RawBlock;

typedef struct {
  Address a;
} Block;

extern Block nullBlock;
extern RawBlock nullRawBlock;

/* address conversions */
Block memRawBlockToBlock(RawBlock rawBlock);
RawBlock memBlockToRawBlock(Block block);
RawBlock memAddrToRawBlock(Address addr);

/* user functions */
Block memAlloc(Word nWords);
void memFree(Block blk);

/* non-user functions */
void memStart();
void memStop();
void memShowBlock(RawBlock blk);
void memDump();

Word memGetNBlocks();
Word memGetNFreeWords();

/* should be private, but they're used by GC */
void memFreeRaw(RawBlock blk);
RawBlock memGetNext(RawBlock addr);
void memSetNext(RawBlock addr, RawBlock next);
Word memGetStatus(RawBlock blk);
void memSetStatus(RawBlock blk, Word status);

#endif
