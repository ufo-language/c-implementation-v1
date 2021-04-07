#include <stdio.h>

#include "d_real.h"
#include "defines.h"
#include "object.h"

Object realNew(float f) {
  Object real = objAlloc(D_Real, 1);
  union {
    Word w;
    float f;
  } u;
  u.f = f;
  objSetData(real, 0, u.w);
  return real;
}

bool realEqual(Object real, Object obj) {
  return realGet(real) == realGet(obj);
}

float realGet(Object real) {
  union {
    Word w;
    float f;
  } u;
  u.w = objGetData(real, 0);
  return u.f;
}

void realShow(Object real, FILE* stream) {
  fprintf(stream, "%f", realGet(real));
}
