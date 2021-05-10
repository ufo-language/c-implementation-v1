#include <stdarg.h>
#include <stdio.h>

#include "d_array.h"
#include "d_int.h"
#include "d_tuple.h"
#include "defines.h"
#include "delegate.h"
#include "eval.h"
#include "globals.h"
#include "hash.h"
#include "object.h"
#include "thread.h"

Word tupleHash_aux(Object tuple);

/*------------------------------------------------------------------*/
Object tupleN(int nElems, ...) {
  va_list argList;
  va_start(argList, nElems);
  Object tup = objAlloc(D_Tuple, TUP_OBJ_SIZE + nElems);
  objSetData(tup, TUP_NELEMS_OFS, nElems);
  for (int n=0; n<nElems; n++) {
    Object elem = va_arg(argList, Object);
    objSetData(tup, TUP_ELEMS_OFS + n, elem.a);
  }
  va_end(argList);
  Word hashCode = tupleHash_aux(tup);
  objSetData(tup, TUP_HASHCODE_OFS, hashCode);
  return tup;
}

/*------------------------------------------------------------------*/
Object tupleFromArray(Object array) {
  Word nElems = arrayCount(array);
  Object tup = objAlloc(D_Tuple, TUP_OBJ_SIZE + nElems);
  objSetData(tup, TUP_NELEMS_OFS, nElems);
  for (Word n=0; n<nElems; n++) {
    objSetData(tup, TUP_ELEMS_OFS + n, arrayGet_unsafe(array, n).a);
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
    Object elem1 = tupleGet_unsafe(tuple, n);
    Object elem2 = tupleGet_unsafe(other, n);
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
    Object elem = tupleGet_unsafe(tuple, n);
    Object newElem = eval(elem, thd);
    arraySet_unsafe(ary, n, newElem);
  }
  Object newTuple = tupleFromArray(ary);
  return newTuple;
}

/*------------------------------------------------------------------*/
void tupleFreeVars(Object tuple, Object freeVarSet) {
  Word nElems = tupleCount(tuple);
  for (Word n=0; n<nElems; n++) {
    Object elem = tupleGet_unsafe(tuple, n);
    objFreeVars(elem, freeVarSet);
  }
}

/*------------------------------------------------------------------*/
Object tupleGet(Object tuple, Word index, Thread* thd) {
  Object elem = tupleGet_unsafe(tuple, index);
  if (elem.a == nullObj.a) {
    Object exn = arrayNew(3);
    arraySet_unsafe(exn, 0, intNew(index));
    arraySet_unsafe(exn, 1, intNew(tupleCount(tuple)));
    arraySet_unsafe(exn, 2, tuple);
    threadThrowException(thd, "Error", "Tuple index out of bounds", exn);
  }
  return elem;
}

/*------------------------------------------------------------------*/
Object tupleGet_unsafe(Object tuple, Word index) {
  Word nElems = tupleCount(tuple);
  if (index >= nElems) {
    return nullObj;
  }
  Object obj = {objGetData(tuple, TUP_ELEMS_OFS + index)};
  return obj;
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
    Object elem = tupleGet_unsafe(tuple, n);
    hashCode = hashRotateLeft(hashCode) ^ elem.a;
  }
  return hashCode ^ hashPrimes(objGetType(tuple));
}

/*------------------------------------------------------------------*/
void tupleMark(Object tuple) {
  Object elems = {objGetData(tuple, TUP_ELEMS_OFS)};
  objMark(elems);
}

/*------------------------------------------------------------------*/
Object tupleMatch(Object tuple, Object other, Object bindingList) {
  Word nElems1 = tupleCount(tuple);
  Word nElems2 = tupleCount(other);
  if (nElems1 != nElems2) {
    return nullObj;
  }
  for (int n=0; n<nElems1; n++) {
    Object elem1 = tupleGet_unsafe(tuple, n);
    Object elem2 = tupleGet_unsafe(other, n);
    bindingList = objMatch(elem1, elem2, bindingList);
    if (bindingList.a == nullObj.a) {
      return nullObj;
    }
  }
  return bindingList;
}

/*------------------------------------------------------------------*/
void tupleShow(Object tuple, FILE* stream) {
  fputc('<', stream);
  Word size = tupleCount(tuple);
  for (uint n=0; n<size; n++) {
    if (n > 0) {
      fprintf(stream, ", ");
    }
    Object elem = tupleGet_unsafe(tuple, n);
    objShow(elem, stream);
  }
  fputc('>', stream);
}
