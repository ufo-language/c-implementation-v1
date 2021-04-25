#ifndef GC_H
#define GC_H

#include "mem.h"
#include "object.h"

void gcCommit(void);
void gcInit(void);
bool gcIsMarked(Object obj);
void gcMarkSweep(void);
void gcReg(Object obj);
void gcSetBlkMarkedFlag(RawBlock blk);
void gcSetObjMarkedFlag(Object obj);
void gcSetRoot(Object root);

#endif
