#include "d_bool.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_nothing.h"
#include "d_symbol.h"
#include "delegate.h"
#include "gc.h"
#include "globals.h"
#include "namespace.h"
#include "object.h"

Object EMPTY_LIST;
Object NOTHING;
Object TRUE;
Object FALSE;
Object GLOBALS;  /* globally-scoped identifiers */
Object SUPER_GLOBALS;  /* currently just 'it' */
Object SYM_ANY;
Object ObjTypeNameSyms[X_Count];
Object GLOBALS_LIST;

Object listCreateEmpty();

void globalsSetup() {
  EMPTY_LIST = listCreateEmpty();
  GLOBALS_LIST = EMPTY_LIST;
  registerGlobal(EMPTY_LIST);
  NOTHING = registerGlobal(nothingNew());
  TRUE = registerGlobal(boolNew(true));
  FALSE = registerGlobal(boolNew(false));
  SUPER_GLOBALS = registerGlobal(hashNew());
  SYM_ANY = registerGlobal(symbolNew("Any"));
  for (int n=0; n<X_Count; n++) {
    ObjTypeNameSyms[n] = registerGlobal(symbolNew(ObjTypeNames[n]));
  }
}

Object registerGlobal(Object obj) {
  GLOBALS_LIST = listNew(obj, GLOBALS_LIST);
  return obj;
}

void namespacesSetup(Thread* thd) {
  GLOBALS = registerGlobal(nsDefineAll(thd));
}

void globalsMark() {
  listMark(GLOBALS_LIST);
}
