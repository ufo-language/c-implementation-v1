#include "d_bool.h"
#include "d_hash.h"
#include "d_nothing.h"
#include "delegate.h"
#include "gc.h"
#include "namespace.h"
#include "object.h"

Object EMPTY_LIST;
Object NOTHING;
Object TRUE;
Object FALSE;
Object GLOBALS;
Object SUPER_GLOBALS;

Object listCreateEmpty();

void globalsSetup() {
  NOTHING = nothingNew();
  EMPTY_LIST = listCreateEmpty();
  TRUE = boolNew(true);
  FALSE = boolNew(false);
  GLOBALS = nsDefineAll();
  SUPER_GLOBALS = hashNew();
}

void globalsMark() {
  gcSetObjMarkedFlag(NOTHING);
  gcSetObjMarkedFlag(EMPTY_LIST);
  gcSetObjMarkedFlag(TRUE);
  gcSetObjMarkedFlag(FALSE);
  objMark(GLOBALS);
  objMark(SUPER_GLOBALS);
}
