#ifndef D_STRINGBUFFER_H
#define D_STRINGBUFFER_H

#include <stdio.h>

#include "defines.h"
#include "object.h"
#include "thread.h"

#define STRBUF_NCHARS_OFS 0
#define STRBUF_READ_INDEX_OFS 1
#define STRBUF_Q_OFS 2
#define STRBUF_OBJ_SIZE 3

Object stringBufferNew();

Word stringBufferCount(Object sb);
void stringBufferMark(Object sb);
Object stringBufferReadChar(Object sb, Thread* thd);
Object stringBufferReadChar_unsafe(Object sb);
Object stringBufferReadUntil(Object sb, char c);
void stringBufferShow(Object sb, FILE* stream);
Object stringBufferToString(Object sb);
void stringBufferWrite(Object sb, Object string);

#endif
