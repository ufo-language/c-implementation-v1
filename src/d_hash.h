#ifndef D_HASH_H
#define D_HASH_H

#include "object.h"
#include "thread.h"

#define HASH_NBINDINGS_OFS 0
#define HASH_LOADINGFACTOR_OFS 1
#define HASH_BUCKETS_OFS 2

Object hashNew(void);

Word hashCount(Object hash);
bool hashEquals(Object hash, Object other);
Object hashEval(Object hash, Thread* thd);
void hashFreeVars(Object hash, Object freeVarSet);
Object hashGet(Object hash, Object key);
void hashPut(Object hash, Object key, Object val);
void hashShow(Object hash, FILE* stream);

#endif

