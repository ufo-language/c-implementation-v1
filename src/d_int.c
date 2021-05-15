#include <stdio.h>

#include "d_array.h"
#include "d_int.h"
#include "d_real.h"
#include "defines.h"
#include "globals.h"
#include "hash.h"
#include "thread.h"

Word intAdd(Word lhs, Word rhs) { return lhs + rhs; }
Word intDivide(Word lhs, Word rhs) { return lhs / rhs; }
Word intModulus(Word lhs, Word rhs) { return lhs % rhs; }
Word intMultiply(Word lhs, Word rhs) { return lhs * rhs; }
Word intSubtract(Word lhs, Word rhs) { return lhs - rhs; }

/*------------------------------------------------------------------*/
Object intBinop(Object integer, Object rhs, WordWordBinOp binOp, Thread* thd) {
  Object res = NOTHING;
  Word lhsWord = intGet(integer);
  Word rhsWord = 0;
  Word valueWord = 0;
  switch (objGetType(rhs)) {
    case D_Int:
      rhsWord = intGet(rhs);
      break;
    case D_Real:
      rhsWord = (Word)realGet(rhs);
      break;
    default: {
        Object exn = arrayN(2, integer, rhs);
        threadThrowException(thd, "Error", "operator '+' can't be applied to a {} and a {}", exn);
      }
      break;
  }
  valueWord = binOp(lhsWord, rhsWord);
  res = intNew(valueWord);
  return res;
}

/*------------------------------------------------------------------*/
Object intNew(int i) {
  Object integer = objAlloc(D_Int, INT_OBJ_SIZE);
  objSetData(integer, 0, i);
  return integer;
}

/*------------------------------------------------------------------*/
bool intEquals(Object integer, Object other) {
  return intGet(integer) == intGet(other);
}

/*------------------------------------------------------------------*/
int intGet(Object integer) {
  return objGetData(integer, 0);
}

/*------------------------------------------------------------------*/
int intHash(Object integer) {
  return intGet(integer) ^ hashPrimes(objGetType(integer));
}

/*------------------------------------------------------------------*/
void intShow(Object integer, FILE* stream) {
  fprintf(stream, "%d", intGet(integer));
}
