#include <stdio.h>
#include <string.h>

#include "d_list.h"
#include "d_queue.h"
#include "d_string.h"
#include "d_stringbuffer.h"
#include "delegate.h"
#include "object.h"

/*------------------------------------------------------------------*/
Word stringBufferCount(Object sb) {
  return objGetData(sb, STRBUF_NCHARS_OFS);
}

/*------------------------------------------------------------------*/
void stringBufferMark(Object sb) {
  Object q = {objGetData(sb, STRBUF_Q_OFS)};
  objMark(q);
}

/*------------------------------------------------------------------*/
Object stringBufferNew() {
  Object sb = objAlloc(D_StringBuffer, STRBUF_OBJ_SIZE);
  objSetData(sb, STRBUF_NCHARS_OFS, 0);
  objSetData(sb, STRBUF_READ_INDEX_OFS, 0);
  Object q = queueNew();
  objSetData(sb, STRBUF_Q_OFS, q.a);
  return sb;
}

/*------------------------------------------------------------------*/
Object stringBufferReadChar(Object sb, Thread* thd) {
  Object string = stringBufferReadChar_unsafe(sb);
  if (string.a == nullObj.a) {
    threadThrowException(thd, "Error", "StringBuffer empty", sb);
  }
  return string;
}

/*------------------------------------------------------------------*/
#include <assert.h>
Object stringBufferReadChar_unsafe(Object sb) {
  Word nChars = objGetData(sb, STRBUF_NCHARS_OFS);
  if (nChars == 0) {
    return nullObj;
  }
  Object q = {objGetData(sb, STRBUF_Q_OFS)};
  Word readIndex = objGetData(sb, STRBUF_READ_INDEX_OFS);
  Object list = queueAsList(q);
  Object string = listGetFirst(list);
  char c = stringGetChar_unsafe(string, readIndex);
  Word strLen = stringCount(string);
  readIndex++;
  if (readIndex == strLen) {
    queueDeq_unsafe(q);
    objSetData(sb, STRBUF_READ_INDEX_OFS, 0);
  }
  else {
    objSetData(sb, STRBUF_READ_INDEX_OFS, readIndex);
  }
  objSetData(sb, STRBUF_NCHARS_OFS, nChars - 1);
  

  char str[2];
  str[0] = c;
  str[1] = '\0';
  return stringNew(str);
}

/*------------------------------------------------------------------*/
Object stringBufferReadUntil(Object sb, char c) {
  (void)sb;
  (void)c;
  return nullObj;
}

/*------------------------------------------------------------------*/
void stringBufferShow(Object sb, FILE* stream) {
  fputs("StringBuffer{\"", stream);\
  Word nChars = stringBufferCount(sb);
  if (nChars > 0) {
    Object q = {objGetData(sb, STRBUF_Q_OFS)};
    Object list = queueAsList(q);
    Object firstString = listGetFirst(list);
    Word readIndex = objGetData(sb, STRBUF_READ_INDEX_OFS);
    while (readIndex < nChars) {
      char c = stringGetChar_unsafe(firstString, readIndex);
      fputc(c, stream);
      readIndex++;
    }
    list = listGetRest(list);
    while (!listIsEmpty(list)) {
      Object string = listGetFirst(list);
      stringShowChars(string, stream);
      list = listGetRest(list);
    }
  }
  fputs("\"}", stream);
}

/*------------------------------------------------------------------*/
void stringBufferWrite(Object sb, Object string) {
  Word nChars = stringCount(string);
  if (nChars > 0) {
    Object q = {objGetData(sb, STRBUF_Q_OFS)};
    queueEnq(q, string);
    objIncDataBy(sb, STRBUF_NCHARS_OFS, nChars);
  }
}
