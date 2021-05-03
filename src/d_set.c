#include "d_array.h"
#include "d_binding.h"
#include "d_set.h"
#include "d_list.h"
#include "delegate.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "object.h"

static void _resize(Object set);

bool setLocate(Object set, Object elem, Word* bucketNum);

/*------------------------------------------------------------------*/
void setAddElem(Object set, Object elem) {
  Word bucketNum;
  if (!setLocate(set, elem, &bucketNum)) {
    Word nElems = objGetData(set, SET_NELEMS_OFS);
    Word loadingFactor = objGetData(set, SET_LOADINGFACTOR_OFS);
    if (nElems == loadingFactor) {
      _resize(set);
      setAddElem(set, elem);
    }
    else {
      Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
      Object bucket = arrayGet(buckets, bucketNum);
      arraySet(buckets, bucketNum, listNew(elem, bucket));
      objIncData(set, SET_NELEMS_OFS);
    }
  }
}

/*------------------------------------------------------------------*/
Word setCount(Object set) {
  return objGetData(set, SET_NELEMS_OFS);
}

/*------------------------------------------------------------------*/
bool setEquals(Object set, Object other) {
  if (setCount(set) != setCount(other)) {
    return false;
  }
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object elem = listGetFirst(bucket);
      if (!setHas(other, elem)) {
        return false;
      }
      bucket = listGetRest(bucket);
    }
  } 
  return true;
}

/*------------------------------------------------------------------*/
Object setEval(Object set, Thread* thd) {
  Object newSet = setNew();
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object elem = listGetFirst(bucket);
      Object elemVal = eval(elem, thd);
      setAddElem(newSet, elemVal);
      bucket = listGetRest(bucket);
    }
  }
  return newSet;
}

/*------------------------------------------------------------------*/
void setFreeVars(Object set, Object freeVarSet) {
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object elem = listGetFirst(bucket);
      objFreeVars(elem, freeVarSet);
      bucket = listGetRest(bucket);
    }
  }  
}

/*------------------------------------------------------------------*/
bool setHas(Object set, Object elem) {
  Word bucketNum;
  return setLocate(set, elem, &bucketNum);
}

/*------------------------------------------------------------------*/
bool setLocate(Object set, Object elem, Word* bucketNum) {
  Word hashCode = objHashCode(elem);
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  *bucketNum = hashCode % nBuckets;
  Object bucket = arrayGet(buckets, *bucketNum);
  while (!listIsEmpty(bucket)) {
    if (objEquals(elem, listGetFirst(bucket))) {
      return true;
    }
    bucket = listGetRest(bucket);
  }
  return false;
}

/*------------------------------------------------------------------*/
void setMark(Object set) {
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  objMark(buckets);
}

/*------------------------------------------------------------------*/
Object setNew(void) {
  Word nBuckets = 8;
  Word loadingFactorCapacity = (nBuckets >> 1) + (nBuckets >> 2);
  Object buckets = arrayNew(nBuckets);
  arrayFill(buckets, EMPTY_LIST);
  Object set = objAlloc(D_Set, SET_OBJ_SIZE);
  objSetData(set, SET_NELEMS_OFS, 0);
  objSetData(set, SET_LOADINGFACTOR_OFS, loadingFactorCapacity);
  objSetData(set, SET_BUCKETS_OFS, buckets.a);
  return set;
}

/*------------------------------------------------------------------*/
bool setRemoveElem(Object set, Object elem) {
  Word bucketNum;
  if (setLocate(set, elem, &bucketNum)) {
    Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
    Object bucket = arrayGet(buckets, bucketNum);
    Object newBucket = EMPTY_LIST;
    while (!listIsEmpty(bucket)) {
      Object elem1 = listGetFirst(bucket);
      if (!objEquals(elem, elem1)) {
        newBucket = listNew(elem1, newBucket);
      }
      bucket = listGetRest(bucket);
    }
    arraySet(buckets, bucketNum, newBucket);
    objDecData(set, SET_NELEMS_OFS);
    return true;
  }
  return false;
}

/*------------------------------------------------------------------*/
void setRemoveSet(Object set, Object otherSet) {
  Object buckets = {objGetData(otherSet, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object elem = listGetFirst(bucket);
      setRemoveElem(set, elem);
      bucket = listGetRest(bucket);
    }
  }
}

/*------------------------------------------------------------------*/
void setShow(Object set, FILE* stream) {
  fputs("${", stream);
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  bool firstIter = true;
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      if (firstIter) {
        firstIter = false;
      }
      else {
        fputs(", ", stream);
      }
      Object elem = listGetFirst(bucket);
      objShow(elem, stream);
      bucket = listGetRest(bucket);
    }
  } 
  fputc('}', stream);
}

/*------------------------------------------------------------------*/
Object setToArray(Object set) {
  Word nElems = setCount(set);
  Object array = arrayNew(nElems);
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  Word elemN = 0;
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object elem = listGetFirst(bucket);
      arraySet(array, elemN++, elem);
      bucket = listGetRest(bucket);
    }
  }
  return array;
}

/*------------------------------------------------------------------*/
void setUnion(Object set, Object otherSet) {
  Object buckets = {objGetData(otherSet, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object elem = listGetFirst(bucket);
      setAddElem(set, elem);
      bucket = listGetRest(bucket);
    }
  } 
}

/*------------------------------------------------------------------*/
static void _resize(Object set) {
  /* allocate new buckets array */
  Object buckets = {objGetData(set, SET_BUCKETS_OFS)};
  Word nBuckets = arrayCount(buckets);
  Word nBucketsNew = (nBuckets * 3) / 2;
  Object bucketsNew = arrayNew(nBucketsNew);
  arrayFill(bucketsNew, EMPTY_LIST);
  /* update set properties */
  objSetData(set, SET_NELEMS_OFS, 0);
  Word loadingFactorCapacityNew = (nBucketsNew >> 1) + (nBucketsNew >> 2);
  objSetData(set, SET_LOADINGFACTOR_OFS, loadingFactorCapacityNew);
  objSetData(set, SET_BUCKETS_OFS, bucketsNew.a);
  /* copy bindings to new set table */
  for (Word n=0; n<nBuckets; n++) {
    Object bucket = arrayGet(buckets, n);
    while (!listIsEmpty(bucket)) {
      Object elem = listGetFirst(bucket);
      setAddElem(set, elem);
      bucket = listGetRest(bucket);
    }
  }
}
