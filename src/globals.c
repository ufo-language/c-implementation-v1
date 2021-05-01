#include "d_nothing.h"
#include "delegate.h"
#include "gc.h"
#include "object.h"

Object EMPTY_LIST;
Object NOTHING;
Object TRAMPOLINE;

Object listCreateEmpty();

void globalsSetup() {
  NOTHING = nothingNew();
  EMPTY_LIST = listCreateEmpty();
}

void globalsMark() {
  gcSetObjMarkedFlag(NOTHING);
  gcSetObjMarkedFlag(EMPTY_LIST);
}
