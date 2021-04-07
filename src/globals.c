#include <stdlib.h>
#include <stdio.h>  /* TODO remove after debugging */

#include "object.h"

Object EMPTY_LIST;
Object NOTHING;

Object nothingNew();
Object listCreateEmpty();

void globalsSetup() {
  NOTHING = nothingNew();
  EMPTY_LIST = listCreateEmpty();
}
