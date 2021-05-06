#include "globals.h"
#include "object.h"

Object io_defineAll(Object env);

Object nsDefineAll() {
  Object env = EMPTY_LIST;
  env = io_defineAll(env);
  return env;
}
