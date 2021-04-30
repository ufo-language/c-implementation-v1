#include "d_nothing.h"
#include "delegate.h"
#include "gc.h"
#include "object.h"
#include "trampoline.h"

Object EMPTY_LIST;
Object NOTHING;
Object TRAMPOLINE;

Object listCreateEmpty();

void globalsSetup() {
  NOTHING = nothingNew();
  EMPTY_LIST = listCreateEmpty();
  TRAMPOLINE = trampNew(nullObj, nullObj);
}

void globalsMark() {
  gcSetObjMarkedFlag(NOTHING);
  gcSetObjMarkedFlag(EMPTY_LIST);
  objMark(TRAMPOLINE);
}
