#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "d_array.h"
#include "d_binding.h"
#include "d_bool.h"
#include "d_closure.h"
#include "d_exn.h"
#include "d_hash.h"
#include "d_int.h"
#include "d_nothing.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_real.h"
#include "d_set.h"
#include "d_string.h"
#include "d_symbol.h"
#include "d_tuple.h"
#include "delegate.h"
#include "e_abstr.h"
#include "e_app.h"
#include "e_ident.h"
#include "e_if.h"
#include "e_let.h"
#include "e_letin.h"
#include "e_letrec.h"
#include "e_quote.h"
#include "e_seq.h"
#include "e_throw.h"
#include "gc.h"
#include "globals.h"
#include "object.h"
#include "trampoline.h"
#include "vmem.h"

/*------------------------------------------------------------------*/
bool objBoolValue(Object obj) {
  ObjType objType = objGetType(obj);
  switch (objType) {
    case D_Array:
      return arrayCount(obj) > 0;
    case D_Bool:
      return boolGet(obj);
    case D_Hash:
      return hashCount(obj) > 0;
    case D_Int:
      return intGet(obj) != 0;
    case D_Nothing:
      return false;
    case D_List:
      return obj.a != EMPTY_LIST.a;
    case D_Queue:
      return queueCount(obj) > 0;
    case D_Real:
      return realGet(obj) != 0;
    case D_Set:
      return setCount(obj) != 0;
    case D_String:
      return stringCount(obj) > 0;
    case D_Tuple:
      return tupleCount(obj) > 0;
    default:
      /* E_Ident, E_If, etc. */
      return true;
  }
}

/*------------------------------------------------------------------*/
Object objCopy(Object obj) {
  RawBlock blk = objToRawBlock(obj);
  Word nWords = memGetSize(blk) - OBJ_OVERHEAD;
  ObjType objType = objGetType(obj);
  Object objCopy = objAlloc(objType, nWords);
  for (int n=0; n<nWords; n++) {
    objSetData(objCopy, n, objGetData(obj, n));
  }
  return objCopy;
}

/*------------------------------------------------------------------*/
void objDisp(Object obj, FILE* stream) {
  switch (objGetType(obj)) {
    case D_String:
      stringDisp(obj, stream);
      break;
    default:
      objShow(obj, stream);
  }
}

/*------------------------------------------------------------------*/
bool objEquals(Object obj1, Object obj2) {
  if (obj1.a == obj2.a) {
    return true;
  }
  ObjType obj1Type = objGetType(obj1);
  ObjType obj2Type = objGetType(obj2);
  if (obj1Type != obj2Type) {
    return false;
  }
  switch (obj1Type) {
    case D_Array:
      return arrayEquals(obj1, obj2);
    case D_Binding:
      return bindingEquals(obj1, obj2);
    case D_Bool:
      return boolEquals(obj1, obj2);
    case D_Exn:
      return exnEquals(obj1, obj2);
    case D_Hash:
      return hashEquals(obj1, obj2);
    case D_Int:
      return intEquals(obj1, obj2);
    case D_List:
      return listEquals(obj1, obj2);
    case D_Queue:
      return queueEquals(obj1, obj2);
    case D_Real:
      return realEquals(obj1, obj2);
    case D_Set:
      return setEquals(obj1, obj2);
    case D_String:
      return stringEquals(obj1, obj2);
    case D_Symbol:
      return symbolEquals(obj1, obj2);
    case D_Tuple:
      return tupleEquals(obj1, obj2);
    case E_Ident:
      return identEquals(obj1, obj2);
    default:
      printf("objEquals not implemented for type %s\n", ObjTypeNames[objGetType(obj1)]);
      printf("  returning default false\n");
  }
  return false;
}

/*------------------------------------------------------------------*/
Object objEval(Object obj, Thread* thd) {
  while (true) {
    switch (objGetType(obj)) {
      case D_Array:
        return arrayEval(obj, thd);
      case D_Binding:
        return bindingEval(obj, thd);
      case D_Exn:
        return exnEval(obj, thd);
      case D_Hash:
        return hashEval(obj, thd);
      case D_List:
        return listEval(obj, thd);
      case D_Queue:
        return queueEval(obj, thd);
      case D_Set:
        return setEval(obj, thd);
      case D_Tuple:
        return tupleEval(obj, thd);
      case E_Abstr:
        return abstrEval(obj, thd);
      case E_App:
        return appEval(obj, thd);
      case E_Ident:
        return identEval(obj, thd);
      case E_If:
        return ifEval(obj, thd);
      case E_Let:
        return letEval(obj, thd);
      case E_LetIn:
        return letInEval(obj, thd);
      case E_LetRec:
        return letRecEval(obj, thd);
      case E_Quote:
        return quoteEval(obj, thd);
      case E_Seq:
        obj = seqEval(obj, thd);
        if (objGetType(obj) != S_Trampoline) {
           return obj;
        }
        break;
      case E_Throw:
        return throwEval(obj, thd);
      case S_Trampoline:
        return trampEval(obj, thd);
      default:
        /* return the object unevaluated */
        return obj;
    }
  }
}

/*------------------------------------------------------------------*/
void objFreeVars(Object obj, Object freeVarSet) {
  switch (objGetType(obj)) {
    case D_Array:
      arrayFreeVars(obj, freeVarSet);
      break;
    case D_Binding:
      bindingFreeVars(obj, freeVarSet);
      break;
    case D_Hash:
      hashFreeVars(obj, freeVarSet);
      break;
    case D_List:
      listFreeVars(obj, freeVarSet);
      break;
    case D_Queue:
      queueFreeVars(obj, freeVarSet);
      break;
    case D_Set:
      setFreeVars(obj, freeVarSet);
      break;
    case D_Tuple:
      tupleFreeVars(obj, freeVarSet);
      break;
    case E_Abstr:
      abstrFreeVars(obj, freeVarSet);
      break;
    case E_App:
      appFreeVars(obj, freeVarSet);
      break;
    case E_Ident:
      setAddElem(freeVarSet, obj);
      break;
    case E_If:
      ifFreeVars(obj, freeVarSet);
      break;
    case E_Let:
      letFreeVars(obj, freeVarSet);
      break;
    case E_LetIn:
      letInFreeVars(obj, freeVarSet);
      break;
    case E_LetRec:
      letRecFreeVars(obj, freeVarSet);
      break;
    case E_Quote:
      quoteFreeVars(obj, freeVarSet);
      break;
    case E_Seq:
      seqFreeVars(obj, freeVarSet);
      break;
    case E_Throw:
      throwFreeVars(obj, freeVarSet);
      break;
    case S_Trampoline: {
        Object obj1 = {objGetData(obj, 0)};
        objFreeVars(obj1, freeVarSet);
      }
      break;
    default:
      ;
  }
}

/*------------------------------------------------------------------*/
Word objHashCode(Object obj) {
  switch (objGetType(obj)) {
    /* TODO finish these cases */
    /*case D_Nothing:
      nothingHash(stream);
      break;*/
    /*case D_Array:
      arrayHash(obj, stream);
      break;*/
    /*case D_Binding:
      bindingHash(obj, stream);
      break;*/
    /*case D_Bool:
      boolHash(obj, stream);
      break;*/
    /*case D_Exn:
      exnHash(obj, stream);
      break;*/
    case D_Int:
      return intHash(obj);
    /*case D_List:
      listHash(obj, stream);
      break;*/
    /*case D_Real:
      realHash(obj, stream);
      break;*/
    case D_String:
      return stringHash(obj);
    case D_Symbol:
      return symbolHash(obj);
    case D_Tuple:
      return tupleHash(obj);
    case E_Ident:
      return identHash(obj);
    default:
      fprintf(stderr, "WARNING: object type %s not handled in objHashCode\n",
        ObjTypeNames[objGetType(obj)]);
  }
  return obj.a;
}

/*------------------------------------------------------------------*/
void objMark_generic(Object obj, Word start, Word count) {
  Word to = start + count;
  for (Word n=start; n<to; n++) {
    Object obj1 = {objGetData(obj, n)};
    objMark(obj1);
  }
}

/*------------------------------------------------------------------*/
void objMark(Object obj) {
  if (gcIsMarked(obj)) {
    return;
  }
  gcSetObjMarkedFlag(obj);
  switch (objGetType(obj)) {
    case D_Array:
      objMark_generic(obj, ARY_ELEMS_OFS, arrayCount(obj));
      break;
    case D_Binding:
      objMark_generic(obj, BND_LHS_OFS, 2);
      break;
    case D_Closure:
      objMark_generic(obj, CLO_PARAMS_OFS, CLO_SIZE);
      break;
    case D_Exn: {
        Object payload = {objGetData(obj, EXN_PAYLOAD_OFS)};
        objMark(payload);
      }
      break;
    case D_Hash: {
        Object buckets = {objGetData(obj, HASH_BUCKETS_OFS)};
        objMark(buckets);
      }
      break;
    case D_List:
      objMark_generic(obj, LST_FIRST_OFS, 2);
      break;
    case D_Queue: {
        Object head = {objGetData(obj, Q_HEAD_OFS)};
        objMark(head);
      }
      break;
    case D_Set: {
        Object buckets = {objGetData(obj, SET_BUCKETS_OFS)};
        objMark(buckets);
      }
      break;
    case D_Tuple:
      objMark_generic(obj, TUP_ELEMS_OFS, tupleCount(obj));
      break;
    case E_Abstr:
      objMark_generic(obj, ABSTR_PARAMS_OFS, ABSTR_SIZE);
      break;
    case E_App:
      objMark_generic(obj, APP_ABSTR_OFS, APP_SIZE);
      break;
    case E_If:
      objMark_generic(obj, IF_COND_OFS, IF_SIZE);
      break;
    case E_Let:
      objMark_generic(obj, LET_BINDINGS_OFS, 1);
      break;
    case E_LetIn:
      objMark_generic(obj, LETIN_BINDINGS_OFS, LETIN_SIZE);
      break;
    case E_LetRec:
      objMark_generic(obj, LETREC_BINDINGS_OFS, 1);
      break;
    case E_Quote:
      objMark_generic(obj, QUOTE_EXPR_OFS, 1);
      break;
    case E_Seq:
      objMark_generic(obj, SEQ_EXPRS_OFS, 1);
      break;
    case E_Throw:
      objMark_generic(obj, THR_PAYLOAD_OFS, 1);
      break;
    case S_Trampoline: {
        Object obj1 = {objGetData(obj, 0)};
        objMark(obj1);
      }
      break;
    default:
      ;  /* do nothing */
  }
}

/*------------------------------------------------------------------*/
Object objMatch(Object obj, Object other, Object bindingList) {
  ObjType objType1 = objGetType(obj);
  if (objType1 == E_Ident) {
    return identMatch(obj, other, bindingList);
  }
  ObjType objType2 = objGetType(other);
  if (objType2 == E_Ident) {
    return identMatch(other, obj, bindingList);
  }
  if (objType1 != objType2) {
    return nullObj;
  }
  switch (objType1) {
    case D_Array:
      return arrayMatch(obj, other, bindingList);
    case D_Binding:
      return bindingMatch(obj, other, bindingList);
    case D_List:
      return listMatch(obj, other, bindingList);
    case D_Tuple:
      return tupleMatch(obj, other, bindingList);
    default:
      return (obj.a == other.a) ? bindingList : nullObj;
  }
}

/*------------------------------------------------------------------*/
Word _getSize(RawBlock blk);
RawBlock objToRawBlock(Object obj);

void objShow(Object obj, FILE* stream) {
  switch (objGetType(obj)) {
    case D_Nothing:
      nothingShow(stream);
      break;
    case D_Array:
      arrayShow(obj, stream);
      break;
    case D_Binding:
      bindingShow(obj, stream);
      break;
    case D_Bool:
      boolShow(obj, stream);
      break;
    case D_Closure:
      closureShow(obj, stream);
      break;
    case D_Exn:
      exnShow(obj, stream);
      break;
    case D_Hash:
      hashShow(obj, stream);
      break;
    case D_Int:
      intShow(obj, stream);
      break;
    case D_List:
      listShow(obj, stream);
      break;
    case D_Queue:
      queueShow(obj, stream);
      break;
    case D_Real:
      realShow(obj, stream);
      break;
    case D_Set:
      setShow(obj, stream);
      break;
    case D_String:
      stringShow(obj, stream);
      break;
    case D_Symbol:
      symbolShow(obj, stream);
      break;
    case D_Tuple:
      tupleShow(obj, stream);
      break;
    case E_Abstr:
      abstrShow(obj, stream);
      break;
    case E_App:
      appShow(obj, stream);
      break;
    case E_Ident:
      identShow(obj, stream);
      break;
    case E_If:
      ifShow(obj, stream);
      break;
    case E_Let:
      letShow(obj, stream);
      break;
    case E_LetIn:
      letInShow(obj, stream);
      break;
    case E_LetRec:
      letRecShow(obj, stream);
      break;
    case E_Quote:
      quoteShow(obj, stream);
      break;
    case E_Seq:
      seqShow(obj, stream);
      break;
    case E_Throw:
      throwShow(obj, stream);
      break;
    case D_Null:
      fprintf(stream, "NULL-OBJECT");
      break;
    case S_Trampoline: {
        fprintf(stream, "Trampoline{");
        Object obj1 = {objGetData(obj, 0)};
        objShow(obj1, stream);
        fputc('}', stream);
      }
      break;
    default:
      fprintf(stream, "SHOW:UNHANDLED-OBJECT(%s)@%d", ObjTypeNames[objGetType(obj)], obj.a);
  }
}
