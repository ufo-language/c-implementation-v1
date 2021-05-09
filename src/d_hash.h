#ifndef D_HASH_H
#define D_HASH_H

#include "object.h"
#include "thread.h"

#define HASH_NBINDINGS_OFS 0
#define HASH_LOADINGFACTOR_OFS 1
#define HASH_BUCKETS_OFS 2
#define HASH_OBJ_SIZE 3

Object hashNew(void);

Word hashCount(Object hash);
bool hashEquals(Object hash, Object other);
Object hashEval(Object hash, Thread* thd);
void hashFreeVars(Object hash, Object freeVarSet);
Object hashGet(Object hash, Object key, Thread* thd);
Object hashGet_unsafe(Object hash, Object key);
Object hashKeys(Object hash);
void hashMark(Object hash);
void hashPut(Object hash, Object key, Object val);
void hashShow(Object hash, FILE* stream);

#endif

