#include <stdio.h>

#include "d_real.h"
#include "defines.h"
#include "object.h"

Object realNew(float f) {
  Object real = objAlloc(D_Real, 2);
  union {
    Word w[2];
    float f;
  } u;
  u.f = f;
  objSetData(real, 0, u.w[0]);
  objSetData(real, 1, u.w[1]);
  return real;
}

bool realEqual(Object real, Object obj) {
  return realGet(real) == realGet(obj);
}

float realGet(Object real) {
  union {
    Word w[2];
    float f;
  } u;
  u.w[0] = objGetData(real, 0);
  u.w[1] = objGetData(real, 1);
  return u.f;
}

void realShow(Object real, FILE* stream) {
  fprintf(stream, "%f", realGet(real));
}
