#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_set.h"
#include "delegate.h"
#include "eval.h"
#include "globals.h"
#include "object.h"

Object hashLocate(Object hash, Object key, Word* bucketNum);

static void _resize(Object hash);

/*------------------------------------------------------------------*/
Word hashCount(Object hash) {
  return objGetData(hash, HASH_NBINDINGS_OFS);
}

/*------------------------------------------------------------------*/
Object hashFold(Object hash, Object data, Object (*fun)(Object data, Object key, Object val)) {
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet_unsafe(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object binding = listGetFirst(bucket);
      Object key = bindingGetLhs(binding);
      Object val = bindingGetRhs(binding);
      Object data1 = fun(data, key, val);
      if (data1.a == nullObj.a) {
        return data;
      }
      data = data1;
      bucket = listGetRest(bucket);
    }
  }
  return data;
}

/*------------------------------------------------------------------*/
bool hashEquals(Object hash, Object other) {
  if (hashCount(hash) != hashCount(other)) {
    return false;
  }
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet_unsafe(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object binding = listGetFirst(bucket);
      Object key = bindingGetLhs(binding);
      Object val = bindingGetRhs(binding);
      if (!objEquals(val, hashGet_unsafe(other, key))) {
        return false;
      }
      bucket = listGetRest(bucket);
    }
  } 
  return true;
}

/*------------------------------------------------------------------*/
Object hashEval(Object hash, Thread* thd) {
  Object newHash = hashNew();
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet_unsafe(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object binding = listGetFirst(bucket);
      Object key = bindingGetLhs(binding);
      Object val = bindingGetRhs(binding);
      Object val1 = eval(val, thd);
      hashPut(newHash, key, val1);
      bucket = listGetRest(bucket);
    }
  }
  return newHash;
}

/*------------------------------------------------------------------*/
Object hashFreeVars_callback(Object freeVarSet, Object key, Object val) {
  objFreeVars(key, freeVarSet);
  objFreeVars(val, freeVarSet);
  return freeVarSet;
}

void hashFreeVars(Object hash, Object freeVarSet) {
  hashFold(hash, freeVarSet, *hashFreeVars_callback);
}

#if 0
/* TODO decide what to do with this, especially w.r.t. hashFold */
void hashFreeVars(Object hash, Object freeVarSet) {
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet_unsafe(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object binding = listGetFirst(bucket);
      Object key = bindingGetLhs(binding);
      Object val = bindingGetRhs(binding);
      objFreeVars(key, freeVarSet);
      objFreeVars(val, freeVarSet);
      bucket = listGetRest(bucket);
    }
  }
}
#endif

/*------------------------------------------------------------------*/
Object hashGet(Object hash, Object key, Thread* thd) {
  Object elem = hashGet_unsafe(hash, key);
  if (elem.a == nullObj.a) {
    Object exn = arrayNew(2);
    arraySet_unsafe(exn, 0, key);
    arraySet_unsafe(exn, 1, hash);
    threadThrowException(thd, "Error", "Key {} not found in hash {}", exn);
  }
  return elem;
}

/*------------------------------------------------------------------*/
Object hashGet_unsafe(Object hash, Object key) {
  Word bucketNum;
  Object binding = hashLocate(hash, key, &bucketNum);
  if (binding.a != nullObj.a) {
    return bindingGetRhs(binding);
  }
  return nullObj;
}

/*------------------------------------------------------------------*/
Object hashKeys_callback(Object keySet, Object key, Object val) {
  (void)val;
  setAddElem(keySet, key);
  return keySet;
}

/*------------------------------------------------------------------*/
Object hashKeys(Object hash) {
  Object keySet = setNew();
  hashFold(hash, keySet, hashKeys_callback);
  return keySet;
}

/*------------------------------------------------------------------*/
void hashMark(Object hash) {
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  objMark(buckets);
}

/*------------------------------------------------------------------*/
Object hashLocate(Object hash, Object key, Word* bucketNum) {
  Word hashCode = objHashCode(key);
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  *bucketNum = hashCode % nBuckets;
  Object bucket = arrayGet_unsafe(buckets, *bucketNum);
  while (!listIsEmpty(bucket)) {
    Object binding = listGetFirst(bucket);
    if (objEquals(key, bindingGetLhs(binding))) {
      return binding;
    }
    bucket = listGetRest(bucket);
  }
  return nullObj;
}

/*------------------------------------------------------------------*/
Object hashNew(void) {
  Word nBuckets = 8;
  Word loadingFactorCapacity = (nBuckets >> 1) + (nBuckets >> 2);
  Object buckets = arrayNew(nBuckets);
  arrayFill(buckets, EMPTY_LIST);
  Object hash = objAlloc(D_Hash, HASH_OBJ_SIZE);
  objSetData(hash, HASH_NBINDINGS_OFS, 0);
  objSetData(hash, HASH_LOADINGFACTOR_OFS, loadingFactorCapacity);
  objSetData(hash, HASH_BUCKETS_OFS, buckets.a);
  return hash;
}

/*------------------------------------------------------------------*/
void hashPut(Object hash, Object key, Object val) {
  Word bucketNum;
  Object binding = hashLocate(hash, key, &bucketNum);
  if (binding.a != nullObj.a) {
    bindingSetRhs(binding, val);
  }
  else {
    Word nBindings = objGetData(hash, HASH_NBINDINGS_OFS);
    Word loadingFactor = objGetData(hash, HASH_LOADINGFACTOR_OFS);
    if (nBindings == loadingFactor) {
      _resize(hash);
      hashPut(hash, key, val);
    }
    else {
      Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
      Object bucket = arrayGet_unsafe(buckets, bucketNum);
      binding = bindingNew(key, val);
      arraySet_unsafe(buckets, bucketNum, listNew(binding, bucket));
      objIncData(hash, HASH_NBINDINGS_OFS);
    }
  }
}

/*------------------------------------------------------------------*/
void hashShow(Object hash, FILE* stream) {
  fputs("#{", stream);
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  bool firstIter = true;
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet_unsafe(buckets, n);
    while (!listIsEmpty(bucket)) {
      if (firstIter) {
        firstIter = false;
      }
      else {
        fputs(", ", stream);
      }
      Object binding = listGetFirst(bucket);
      objShow(binding, stream);
      bucket = listGetRest(bucket);
    }
  } 
  fputc('}', stream);
}

/*------------------------------------------------------------------*/
static void _resize(Object hash) {
  /* allocate new buckets array */
  Object buckets = {objGetData(hash, HASH_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  Word nBucketsNew = (nBuckets * 3) / 2;
  Object bucketsNew = arrayNew(nBucketsNew);
  arrayFill(bucketsNew, EMPTY_LIST);
  /* update hash properties */
  objSetData(hash, HASH_NBINDINGS_OFS, 0);
  /* a loading factor of 0.75 triggers a resize */
  Word loadingFactorCapacityNew = (nBucketsNew >> 1) + (nBucketsNew >> 2);
  objSetData(hash, HASH_LOADINGFACTOR_OFS, loadingFactorCapacityNew);
  objSetData(hash, HASH_BUCKETS_OFS, bucketsNew.a);
  /* copy bindings to new hash table */
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet_unsafe(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object binding = listGetFirst(bucket);
      hashPut(hash, bindingGetLhs(binding), bindingGetRhs(binding));
      bucket = listGetRest(bucket);
    }
  }
}
