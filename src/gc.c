#include <assert.h>

#include "delegate.h"
#include "gc.h"
#include "globals.h"
#include "mem.h"
#include "object.h"

/* The spine contains a singly-linked list of all objects that have
   been allocated, whether or not they are reachable */
RawBlock _gcSpine;
/* The spine needs to remember a single "prev" object, which is the
   final object in the "new" list. */
RawBlock _gcSpinePrev;
/* The new objects are objects that have been allocated but not yet
   committed */
RawBlock _gcNew;

bool _isMarked(RawBlock blk);
void _sweep(void);
void _unmark(RawBlock obj);

long nMarkSweeps = 0;

void gcDumpSpine(void);

/*------------------------------------------------------------------*/
void gcInit(void) {
  assert(nullRawBlock.a == 0);
  _gcSpine = nullRawBlock;
  _gcSpinePrev = nullRawBlock;
  _gcNew = nullRawBlock;
}

/*------------------------------------------------------------------*/
bool gcIsMarked(Object obj) {
  RawBlock blk = objToRawBlock(obj);
  return _isMarked(blk);
}

/*------------------------------------------------------------------*/
void gcCommit(void) {
  _gcSpine = _gcNew;
  _gcSpinePrev = nullRawBlock;
}

/*------------------------------------------------------------------*/
void gcReg(Object obj) {
  RawBlock blk = objToRawBlock(obj);
  memSetNext(blk, _gcNew);
  _gcNew = blk;
  if (_gcSpinePrev.a == 0) {
    _gcSpinePrev = blk;
  }
}

/*------------------------------------------------------------------*/
void gcMarkSweep(void) {
  nMarkSweeps += 1;
  globalsMark();
  threadMarkAllThreads();
  _sweep();
}

/*------------------------------------------------------------------*/
void gcDumpSpine(void) {
  printf("  +================\n");
  printf("  | gcDumpSpine:\n");
  printf("  | _gcNew   = %d\n", _gcNew.a);
  printf("  | _gcSpine = %d\n", _gcSpine.a);
  int n = 0;
  bool isNew = true;
  RawBlock blk = _gcNew;
  while (blk.a && n < 15) { /* shows a maximum of 15 blocks */
    if (blk.a == _gcSpine.a) { isNew = false; }
    printf("  | [%s] %d. ", (isNew ? "NEW" : "   "), n++);
    objShow(objRawBlockToObj(blk), stdout);
    printf("\n");
    blk = memGetNext(blk);
  }
  if (blk.a) printf("  | there are more blocks...\n");
  printf("  +================\n");
}

/*------------------------------------------------------------------*/
void gcSetBlkMarkedFlag(RawBlock blk) {
  memSetStatus(blk, memGetStatus(blk) | MEMBLK_GC_MARK);
}

/*------------------------------------------------------------------*/
void gcSetObjMarkedFlag(Object obj) {
  RawBlock blk = objToRawBlock(obj);
  gcSetBlkMarkedFlag(blk);
}

/*------------------------------------------------------------------*/
bool _isMarked(RawBlock blk) {
  return memGetStatus(blk) >= MEMBLK_GC_MARK;
}

/*------------------------------------------------------------------*/
/* Sweep the spine. Only blocks committed to the spine are swept.
   New/uncommitted blocks are not swept. */
void _sweep(void) {
  RawBlock blk = _gcNew;
  RawBlock prev = nullRawBlock;
  RawBlock next;

  /* unmark the new blocks */
  while (blk.a && blk.a != _gcSpine.a) {
    next = memGetNext(blk);
    _unmark(blk);
    prev = blk;
    blk = next;
  }

  /* sweep the spine */
  while (blk.a) {
    next = memGetNext(blk);

    if (_isMarked(blk)) {
      /* unmark the block and move to the next block */
      _unmark(blk);
      prev = blk;
    }
    else {
#if 0
      Object obj = objRawBlockToObj(blk);
      printf("sweep is deleting "); objShow(obj, stdout); printf("\n");
#endif
      /* remove the block from the spine */
      memFreeRaw(blk);
      if (prev.a == nullRawBlock.a) {
        _gcSpine = next;
        _gcNew = next;
      }
      else {
        memSetNext(prev, next);
      }
    }

    /* bottom of loop */
    blk = next;
  }
}

/*------------------------------------------------------------------*/
void _unmark(RawBlock blk) {
  Word objStatus = memGetStatus(blk);
  if (objStatus >= MEMBLK_GC_MARK) {
    memSetStatus(blk, objStatus & !MEMBLK_GC_MARK);
  }
}
