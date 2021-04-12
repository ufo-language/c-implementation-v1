#ifndef SYNTAX_H
#define SYNTAX_H

#include "lexer.h"

char* RESERVED_WORDS[] = {
  "do", "else", "end", "fun", "if", "then", "while",
  0
};

char* BOOL_WORDS[] = {
  "false", "true",
  0
};

/* S_I */
Transition initialState[] = {
  {'a', 'z',     S_WORD,  A_KEEP,      T_NONE},
  {'A', 'Z',     S_WORD,  A_KEEP,      T_NONE},
  {'0', '9',     S_INT,   A_KEEP,      T_NONE},
  {'"', '"',     S_STR,   A_IGNORE,    T_NONE},
  {' ', ' ',     S_I,     A_IGNORE,    T_NONE},
  {'\n', '\n',   S_I,     A_IGNORE,    T_NONE},
  {'+', '+',     S_OPER,  A_KEEP,      T_NONE},
  {'-', '-',     S_OPER,  A_KEEP,      T_NONE},
  {'*', '*',     S_OPER,  A_KEEP,      T_NONE},
  {'/', '/',     S_OPER,  A_KEEP,      T_NONE},
  {'<', '>',     S_OPER,  A_KEEP,      T_NONE},
  {'!', '!',     S_OPER,  A_KEEP,      T_NONE},
  {C_NUL, C_NUL, S_I,     A_IGNORE,    T_EOI},     /* C_NUL is end of input */
  {C_ANY, C_ANY, S_I,     A_KEEP,      T_SPECIAL}  /* C_ANY is wildcard */
};

/* S_INT */
Transition integerState[] = {
  {'0', '9',     S_INT,   A_KEEP,      T_NONE},
  {'.', '.',     S_POINT, A_KEEP,      T_NONE},
  {C_ANY, C_ANY, S_I,     A_REUSE,     T_INT}
};

/* S_POINT: transition from INT to REAL */
Transition pointState[] = {
  {'0', '9',     S_REAL,  A_KEEP,      T_NONE},
  {C_ANY, C_ANY, S_I,     A_ERR_REAL,  T_NONE}
};

/* S_REAL */
Transition realState[] = {
  {'0', '9',     S_REAL,  A_KEEP,      T_NONE},
  {C_ANY, C_ANY, S_I,     A_REUSE,     T_REAL}
};

/* S_WORD */
Transition wordState[] = {
  {'a', 'z',     S_WORD,  A_KEEP,      T_NONE},
  {'A', 'Z',     S_WORD,  A_KEEP,      T_NONE},
  {'_', '_',     S_WORD,  A_KEEP,      T_NONE},
  {C_ANY, C_ANY, S_I,     A_REUSE,     T_WORD}
};

/* S_STR */
Transition stringState[] = {
  {'"', '"',     S_I,     A_IGNORE,    T_STRING},
  {C_NUL, C_NUL, S_I,     A_ERR_STRING,T_NONE},
  {C_ANY, C_ANY, S_STR,   A_KEEP,      T_NONE}
};

/* S_OPER */
Transition operatorState[] = {
  {'+', '+',     S_OPER,  A_KEEP,      T_NONE},
  {'-', '-',     S_OPER,  A_KEEP,      T_NONE},
  {'*', '*',     S_OPER,  A_KEEP,      T_NONE},
  {'/', '/',     S_OPER,  A_KEEP,      T_NONE},
  {'<', '>',     S_OPER,  A_KEEP,      T_NONE},
  {'!', '!',     S_OPER,  A_KEEP,      T_NONE},
  {C_ANY, C_ANY, S_I,     A_REUSE,     T_OPER}
};

/* the order of the elements in this array must match the order
   of the enumerations in the StateName enum in lexer.h */
Transition* syntax[] = {
  initialState,
  integerState,
  pointState,
  realState,
  wordState,
  stringState,
  operatorState
};

#endif
