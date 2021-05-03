#include <stdio.h>

#include "d_nothing.h"
#include "object.h"

Object nothingNew(void) {
  Object nothing = objAlloc(D_Nothing, NTH_OBJ_SIZE);
  return nothing;
}

void nothingShow(FILE* stream) {
  fprintf(stream, "nothing");
}
