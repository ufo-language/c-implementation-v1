#include <stdio.h>

#include "d_seq.h"
#include "d_int.h"
#include "delegate.h"
#include "eval.h"
#include "object.h"
#include "thread.h"

/*------------------------------------------------------------------*/
Object seqNew(Object from, Object to, Object step) {
  Object seq = objAlloc(D_Seq, SEQ_OBJ_SIZE);
  objSetData(seq, SEQ_FROM_OFS, from.a);
  objSetData(seq, SEQ_TO_OFS, to.a);
  objSetData(seq, SEQ_STEP_OFS, step.a);
  return seq;
}

/*------------------------------------------------------------------*/
bool seqEquals(Object seq, Object other) {
  Object from1 = seqGetFrom(seq);
  Object from2 = seqGetFrom(other);
  if (!objEquals(from1, from2)) {
    return false;
  }
  Object to1 = seqGetTo(seq);
  Object to2 = seqGetTo(other);
  if (!objEquals(to1, to2)) {
    return false;
  }
  Object step1 = seqGetStep(seq);
  Object step2 = seqGetStep(other);
  return objEquals(step1, step2);
}

/*------------------------------------------------------------------*/
Object seqGetFrom(Object seq) {
  Object obj = {objGetData(seq, SEQ_FROM_OFS)};
  return obj;
}

/*------------------------------------------------------------------*/
Object seqGetTo(Object seq) {
  Object obj = {objGetData(seq, SEQ_TO_OFS)};
  return obj;
}

/*------------------------------------------------------------------*/
Object seqGetStep(Object seq) {
  Object obj = {objGetData(seq, SEQ_STEP_OFS)};
  return obj;
}

/*------------------------------------------------------------------*/
void seqMark(Object seq) {
  objMark(seqGetFrom(seq));
  objMark(seqGetTo(seq));
  objMark(seqGetStep(seq));
}

/*------------------------------------------------------------------*/
void seqShow(Object seq, FILE* stream) {
  objShow(seqGetFrom(seq), stream);
  fputs("..", stream);
  objShow(seqGetTo(seq), stream);
  Object step = {objGetData(seq, SEQ_STEP_OFS)};
  if (intGet(step) != 1) {
    fputs(" % ", stream);
    objShow(step, stream);
  }
}

