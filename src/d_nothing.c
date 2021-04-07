#include <stdio.h>

#include "d_nothing.h"
#include "object.h"

Object nothingNew() {
  Object nothing = objAlloc(D_Nothing, 0);
  return nothing;
}

void nothingShow(FILE* stream) {
  fprintf(stream, "nothing");
}
