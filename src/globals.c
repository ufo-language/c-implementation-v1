#include "d_nothing.h"
#include "delegate.h"
#include "gc.h"
#include "namespace.h"
#include "object.h"

Object EMPTY_LIST;
Object NOTHING;
Object STANDARD_ENV;

Object listCreateEmpty();

void globalsSetup() {
  NOTHING = nothingNew();
  EMPTY_LIST = listCreateEmpty();
  STANDARD_ENV = nsDefineAll();
}

void globalsMark() {
  gcSetObjMarkedFlag(NOTHING);
  gcSetObjMarkedFlag(EMPTY_LIST);
}
