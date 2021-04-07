#ifndef OBJECT_H
#define OBJECT_H

#include "defines.h"
#include "mem.h"
#include "vmem.h"

/* There must be a 1-to-1 correspondence with the ObjTypeNames strings in
   object.c */
typedef enum {
  D_Null = 0,
  D_Array,
  D_Binding,
  D_Bool,
  D_Closure,
  D_Exn,
  D_Hash,
  D_Int,
  D_List,
  D_Nothing,
  D_Queue,
  D_Real,
  D_Set,
  D_String,
  D_Symbol,
  E_Abstr,
  E_App,
  E_Ident,
  E_If,
  E_Let,
  E_LetIn,
  E_Seq,
  E_Throw,
  D_Unknown /* used only during parsing */
} ObjType;

extern char* ObjTypeNames[];

typedef struct {
  Address a;
} Object;

extern Object nullObj;

/* function prototypes ---------------------------------------------*/

/* pointer conversions */
Object objRawBlockToObj(RawBlock rawBlk);
Object objBlockToObj(Block blk);
Block objToBlock(Object obj);
RawBlock objToRawBlock(Object obj);

/* Allocates nWords and sets its type. If there is no free block large
   enough, gcMarkSweep() will be called. */
Object objAlloc(ObjType type, Word nWords);

/* Allocates nWords and sets its type.
   Returns > 0 on success, or 0 on OOM. */
Object objAlloc_unsafe(ObjType type, Word nWords);
void objFree(Object obj);

ObjType objGetType(Object obj);

Word objGetData(Object obj, Word offset);
void objSetData(Object obj, Word offset, Word value);

void objIncData(Object obj, Word offset);
void objDecData(Object obj, Word offset);

#endif
