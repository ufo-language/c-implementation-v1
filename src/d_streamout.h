#ifndef D_STREAM_OUT_H
#define D_STREAM_OUT_H

#include "object.h"

Object streamOutNew(Object sink);

void streamOutWrite(Object stream, Object obj);

void streamOutMark(Object stream);

#endif
