#include <stdarg.h>
#include <stdio.h>

#include "d_array.h"
#include "d_int.h"
#include "defines.h"
#include "delegate.h"
#include "eval.h"
#include "globals.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object arrayNew(Word nElems) {
  Object array = objAlloc(D_Array, ARY_OBJ_SIZE + nElems);
  objSetData(array, ARY_NELEMS_OFS, nElems);
  for (Word n=0; n<nElems; n++) {
    arraySet_unsafe(array, n, NOTHING);
  }
  return array;
}

/*------------------------------------------------------------------*/
Object arrayN(int nElems, ...) {
  va_list argList;
  va_start(argList, nElems);
  Object array = arrayNew(nElems);
  for (int n=0; n<nElems; n++) {
    Object elem = va_arg(argList, Object);
    arraySet_unsafe(array, n, elem);
  }
  va_end(argList);
  return array;
}

/*------------------------------------------------------------------*/
Word arrayCount(Object array) {
  return objGetData(array, ARY_NELEMS_OFS);
}

/*------------------------------------------------------------------*/
void arrayEach(Object array, void (*fun)(Object data, Object elem), Object data) {
  Word nElems = arrayCount(array);
  for (Word n=0; n<nElems; n++) {
    Object elem = arrayGet_unsafe(array, n);
    fun(data, elem);
  }
}

/*------------------------------------------------------------------*/
bool arrayEquals(Object array, Object other) {
  Word len1 = arrayCount(array);
  Word len2 = arrayCount(other);
  if (len1 != len2) {
    return false;
  }
  for (Word n=0; n<len1; n++) {
    Object elem1 = arrayGet_unsafe(array, n);
    Object elem2 = arrayGet_unsafe(other, n);
    if (!objEquals(elem1, elem2)) {
      return false;
    }
  }
  return true;
}

/*------------------------------------------------------------------*/
Object arrayEval(Object array, Thread* thd) {
  Word nElems = arrayCount(array);
  Object newArray = arrayNew(nElems);
  for (Word n=0; n<nElems; n++) {
    Object elem = arrayGet_unsafe(array, n);
    Object newElem = eval(elem, thd);
    arraySet_unsafe(newArray, n, newElem);
  }
  return newArray;
}

/*------------------------------------------------------------------*/
void arrayFill(Object array, Object elem) {
  Word count = arrayCount(array);
  for (Word n=0; n<count; n++) {
    arraySet_unsafe(array, n, elem);
  }
}

/*------------------------------------------------------------------*/
void arrayFreeVars(Object array, Object freeVarSet) {
  Word nElems = arrayCount(array);
  for (Word n=0; n<nElems; n++) {
    Object elem = arrayGet_unsafe(array, n);
    objFreeVars(elem, freeVarSet);
  }
}

/*------------------------------------------------------------------*/
Object arrayGet(Object array, Word index, Thread* thd) {
  Object elem = arrayGet_unsafe(array, index);
  if (elem.a == nullObj.a) {
    Object exn = arrayN(3, intNew(index), intNew(arrayCount(array)), array);
    threadThrowException(thd, "Error", "Array index out of bounds", exn);
  }
  return elem;
}

/*------------------------------------------------------------------*/
Object arrayGet_unsafe(Object array, Word index) {
  Word nElems = arrayCount(array);
  if (index >= nElems) {
    return nullObj;
  }
  Object obj = {objGetData(array, ARY_ELEMS_OFS + index)};
  return obj;
}

/*------------------------------------------------------------------*/
void arrayMark(Object array) {
  Word nElems = arrayCount(array);
  for (int n=0; n<nElems; n++) {
    Object elem = arrayGet_unsafe(array, n);
    objMark(elem);
  }
}

/*------------------------------------------------------------------*/
Object arrayMatch(Object array, Object other, Object bindingList) {
  Word nElems1 = arrayCount(array);
  Word nElems2 = arrayCount(other);
  if (nElems1 != nElems2) {
    return nullObj;
  }
  for (int n=0; n<nElems1; n++) {
    Object elem1 = arrayGet_unsafe(array, n);
    Object elem2 = arrayGet_unsafe(other, n);
    bindingList = objMatch(elem1, elem2, bindingList);
    if (bindingList.a == nullObj.a) {
      return nullObj;
    }
  }
  return bindingList;
}

/*------------------------------------------------------------------*/
void arraySet(Object array, Word index, Object obj, Thread* thd) {
  bool res = arraySet_unsafe(array, index, obj);
  if (!res) {
    Object exn = arrayN(3, intNew(index), intNew(arrayCount(array)), array);
    threadThrowException(thd, "Error", "Array index out of bounds", exn);
  }
}

/*------------------------------------------------------------------*/
bool arraySet_unsafe(Object array, Word index, Object obj) {
  Word nElems = arrayCount(array);
  if (index >= nElems) {
    return false;
  }
  objSetData(array, ARY_ELEMS_OFS + index, obj.a);
  return true;
}

/*------------------------------------------------------------------*/
void arrayShow(Object array, FILE* stream) {
  fputc('{', stream);
  Word size = arrayCount(array);
  for (uint n=0; n<size; n++) {
    if (n > 0) {
      fprintf(stream, ", ");
    }
    Object elem = arrayGet_unsafe(array, n);
    objShow(elem, stream);
  }
  fputc('}', stream);
}
