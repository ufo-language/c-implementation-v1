#ifndef D_INT_H
#define D_INT_H

#include "defines.h"
#include "object.h"
#include "thread.h"

#define INT_OBJ_SIZE 1

Object intNew(int i);

typedef Word (*WordWordBinOp)(Word lhs, Word rhs);

Word intAdd(Word lhs, Word rhs);
Word intDivide(Word lhs, Word rhs);
Word intModulus(Word lhs, Word rhs);
Word intMultiply(Word lhs, Word rhs);
Word intSubtract(Word lhs, Word rhs);

Object intBinop(Object integer, Object rhs, WordWordBinOp binOp, Thread* thd);

bool intEquals(Object integer, Object other);
int intGet(Object integer);
int intHash(Object integer);
void intShow(Object integer, FILE* stream);

#endif
