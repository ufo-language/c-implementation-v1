#include "d_closure.h"
#include "d_list.h"
#include "d_set.h"
#include "delegate.h"
#include "e_abstr.h"
#include "e_seq.h"
#include "object.h"
#include "thread.h"

void abstrShow_aux(Object abstr, char* prefix, FILE* stream);

Object abstrEval(Object abstr, Thread* thd) {
  Object lexEnv = threadGetEnv(thd);
  Object closure = closureNew(abstr, lexEnv);
  return closure;
}

/* The free vars of a rule are the free vars of the body minus the
   free vars of the parameters.
*/
void abstrFreeVars_rule(Object rule, Object freeVarSet) {
  Object params = {objGetData(rule, ABSTR_PARAMS_OFS)};
  Object body = {objGetData(rule, ABSTR_BODY_OFS)};
  Object paramFreeVars = setNew();
  objFreeVars(params, paramFreeVars);
  Object bodyFreeVars = setNew();
  objFreeVars(body, bodyFreeVars);
  setRemoveSet(bodyFreeVars, paramFreeVars);
  setUnion(freeVarSet, bodyFreeVars);
}

/* The free vars of an abstraction are the union of the free vars of
   all the rules.
*/
void abstrFreeVars(Object abstr, Object freeVarSet) {
  while (abstr.a != nullObj.a) {
    abstrFreeVars_rule(abstr, freeVarSet);
    abstr.a = objGetData(abstr, ABSTR_NEXT_OFS);
  }
}

Object abstrNew(Object params, Object body) {
  /* the body is a list of expressions, convert it to a do/end
     expression */
  body = seqNew(body);
  Object abstr = objAlloc(E_Abstr, ABSTR_OBJ_SIZE);
  objSetData(abstr, ABSTR_PARAMS_OFS, params.a);
  objSetData(abstr, ABSTR_BODY_OFS, body.a);
  objSetData(abstr, ABSTR_NEXT_OFS, nullObj.a);
  return abstr;
}

Object abstrGetBody(Object abstr) {
  Object body = {objGetData(abstr, ABSTR_BODY_OFS)};
  return body;
}

Object abstrGetNext(Object abstr) {
  Object nextRule = {objGetData(abstr, ABSTR_NEXT_OFS)};
  return nextRule;
}

Object abstrGetParams(Object abstr) {
  Object params = {objGetData(abstr, ABSTR_PARAMS_OFS)};
  return params;
}

void abstrMark(Object abstr) {
  Object params = {objGetData(abstr, ABSTR_PARAMS_OFS)};
  Object body = {objGetData(abstr, ABSTR_BODY_OFS)};
  Object next = {objGetData(abstr, ABSTR_NEXT_OFS)};
  objMark(params);
  objMark(body);
  if (next.a != nullObj.a) {
    objMark(next);
  }
}

void abstrSetNext(Object abstr, Object nextRule) {
  objSetData(abstr, ABSTR_NEXT_OFS, nextRule.a);
}

void abstrShow(Object abstr, FILE* stream) {
  abstrShow_aux(abstr, "fun", stream);
}

void abstrShow_aux(Object abstr, char* prefix, FILE* stream) {
  fputs(prefix, stream);
  bool firstIter = true;
  while (abstr.a != nullObj.a) {
    if (firstIter) {
      firstIter = false;
    }
    else {
      fputs("| ", stream);
    }
    Object params = {objGetData(abstr, ABSTR_PARAMS_OFS)};
    Object body = {objGetData(abstr, ABSTR_BODY_OFS)};
    listShowWith(params, "(", ", ", ") = ", stream);
    seqShowWith("", body, " ", stream);
    abstr.a = objGetData(abstr, ABSTR_NEXT_OFS);
  }
  fputs("end", stream);
}
