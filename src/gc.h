#ifndef GC_H
#define GC_H

#include "mem.h"
#include "object.h"

void gcCommit();
void gcInit();
bool gcIsMarked(Object obj);
void gcMarkSweep();
void gcReg(Object obj);
void gcSetMarkFlag(Object obj);
void gcSetBlkMarkedFlag(RawBlock blk);
void gcSetObjMarkedFlag(Object obj);
void gcSetRoot(Object root);

#endif
