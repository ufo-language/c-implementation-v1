#include <stdio.h>

#include "d_array.h"
#include "d_tuple.h"
#include "defines.h"
#include "delegate.h"
#include "eval.h"
#include "gc.h"
#include "globals.h"
#include "hash.h"
#include "object.h"
#include "thread.h"

Word tupleHash_aux(Object tuple);

/*------------------------------------------------------------------*/
Object tuple1(Object elem0) {
  Word nElems = 1;
  Object tup = objAlloc(D_Tuple, TUP_ELEMS_OFS + nElems);
  objSetData(tup, TUP_NELEMS_OFS, nElems);
  objSetData(tup, TUP_ELEMS_OFS, elem0.a);
  Word hashCode = tupleHash_aux(tup);
  objSetData(tup, TUP_HASHCODE_OFS, hashCode);
  return tup;
}

/*------------------------------------------------------------------*/
Object tuple2(Object elem0, Object elem1) {
  Word nElems = 2;
  Object tup = objAlloc(D_Tuple, TUP_ELEMS_OFS + nElems);
  objSetData(tup, TUP_NELEMS_OFS, nElems);
  objSetData(tup, TUP_ELEMS_OFS, elem0.a);
  objSetData(tup, TUP_ELEMS_OFS + 1, elem1.a);
  Word hashCode = tupleHash_aux(tup);
  objSetData(tup, TUP_HASHCODE_OFS, hashCode);
  return tup;
}

/*------------------------------------------------------------------*/
Object tuple3(Object elem0, Object elem1, Object elem2) {
  Word nElems = 3;
  Object tup = objAlloc(D_Tuple, TUP_ELEMS_OFS + nElems);
  objSetData(tup, TUP_NELEMS_OFS, nElems);
  objSetData(tup, TUP_ELEMS_OFS, elem0.a);
  objSetData(tup, TUP_ELEMS_OFS + 1, elem1.a);
  objSetData(tup, TUP_ELEMS_OFS + 2, elem2.a);
  Word hashCode = tupleHash_aux(tup);
  objSetData(tup, TUP_HASHCODE_OFS, hashCode);
  return tup;
}

/*------------------------------------------------------------------*/
Object tuple4(Object elem0, Object elem1, Object elem2, Object elem3) {
  Word nElems = 3;
  Object tup = objAlloc(D_Tuple, TUP_ELEMS_OFS + nElems);
  objSetData(tup, TUP_NELEMS_OFS, nElems);
  objSetData(tup, TUP_ELEMS_OFS, elem0.a);
  objSetData(tup, TUP_ELEMS_OFS + 1, elem1.a);
  objSetData(tup, TUP_ELEMS_OFS + 2, elem2.a);
  objSetData(tup, TUP_ELEMS_OFS + 3, elem3.a);
  Word hashCode = tupleHash_aux(tup);
  objSetData(tup, TUP_HASHCODE_OFS, hashCode);
  return tup;
}

/*------------------------------------------------------------------*/
Object tupleFromArray(Object array) {
  Word nElems = arrayCount(array);
  Object tup = objAlloc(D_Tuple, TUP_ELEMS_OFS + nElems);
  objSetData(tup, TUP_NELEMS_OFS, nElems);
  for (Word n=0; n<nElems; n++) {
    objSetData(tup, TUP_ELEMS_OFS + n, arrayGet(array, n).a);
  }
  Word hashCode = tupleHash_aux(tup);
  objSetData(tup, TUP_HASHCODE_OFS, hashCode);
  return tup;
}

/*------------------------------------------------------------------*/
Word tupleCount(Object tuple) {
  return objGetData(tuple, TUP_NELEMS_OFS);
}

/*------------------------------------------------------------------*/
bool tupleEquals(Object tuple, Object other) {
  Word len1 = tupleCount(tuple);
  Word len2 = tupleCount(other);
  if (len1 != len2) {
    return false;
  }
  for (Word n=0; n<len1; n++) {
    Object elem1 = tupleGet(tuple, n);
    Object elem2 = tupleGet(other, n);
    if (!objEquals(elem1, elem2)) {
      return false;
    }
  }
  return true;
}

/*------------------------------------------------------------------*/
Object tupleEval(Object tuple, Thread* thd) {
  Word nElems = tupleCount(tuple);
  Object ary = arrayNew(tupleCount(tuple));
  for (Word n=0; n<nElems; n++) {
    Object elem = tupleGet(tuple, n);
    Object newElem = eval(elem, thd);
    arraySet(ary, n, newElem);
  }
  Object newTuple = tupleFromArray(ary);
  return newTuple;
}

/*------------------------------------------------------------------*/
void tupleFreeVars(Object tuple, Object freeVarSet) {
  Word nElems = tupleCount(tuple);
  for (Word n=0; n<nElems; n++) {
    Object elem = tupleGet(tuple, n);
    objFreeVars(elem, freeVarSet);
  }
}

/*------------------------------------------------------------------*/
Object tupleGet(Object tuple, Word index) {
  Word nElems = tupleCount(tuple);
  if (index < nElems) {
    Object obj = {objGetData(tuple, TUP_ELEMS_OFS + index)};
    return obj;
  }
  fprintf(stderr, "ERROR: Tuple access violation\n");
  fprintf(stderr, "  tuple: ");
  objShow(tuple, stderr);
  fprintf(stderr, "\n  size: %d\n", tupleCount(tuple));
  fprintf(stderr, "  index requested: %d\n", index);
  return nullObj;
}

/*------------------------------------------------------------------*/
/* Returns the tuple's cached hash code. */
Word tupleHash(Object tuple) {
  Word hashCode = objGetData(tuple, TUP_HASHCODE_OFS);
  return hashCode;
}

/*------------------------------------------------------------------*/
/* Calculates a tuple's hash code. */
Word tupleHash_aux(Object tuple) {
  Word hashCode = 0;
  Word nElems = tupleCount(tuple);
  for (Word n=0; n<nElems; n++) {
    Object elem = tupleGet(tuple, n);
    hashCode = hashRotateLeft(hashCode) ^ elem.a;
  }
  return hashCode ^ hashPrimes(objGetType(tuple));
}

/*------------------------------------------------------------------*/
Object tupleMatch(Object tuple, Object other, Object bindingList) {
  Word nElems1 = tupleCount(tuple);
  Word nElems2 = tupleCount(other);
  if (nElems1 != nElems2) {
    return nullObj;
  }
  for (int n=0; n<nElems1; n++) {
    Object elem1 = tupleGet(tuple, n);
    Object elem2 = tupleGet(other, n);
    bindingList = objMatch(elem1, elem2, bindingList);
    if (bindingList.a == nullObj.a) {
      return nullObj;
    }
  }
  return bindingList;
}

/*------------------------------------------------------------------*/
void tupleShow(Object tuple, FILE* stream) {
  Word size = tupleCount(tuple);
  for (uint n=0; n<size; n++) {
    if (n > 0) {
      fprintf(stream, ":");
    }
    Object elem = tupleGet(tuple, n);
    objShow(elem, stream);
  }
}
