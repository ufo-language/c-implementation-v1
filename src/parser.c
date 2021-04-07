#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "lexer.h"
#include "parser.h"

void foo() {
  // putting a function here to keep the linker from warning that this file is empty
}

#if 0

#define DEBUG 1

typedef Object* (*Parser)(LexerState* lexState);

Object* p_spot(LexerState* lexState, TokenType tt, Token* token);
Object* p_int(LexerState* lexState);
Object* p_real(LexerState* lexState);
Object* p_number(LexerState* lexState);
Object* p_string(LexerState* lexState);
Object* p_object(LexerState* lexState);

/* parser entry function */
Object* parse(char* inputString, Transition** syntax) {
  LexerState lexState;
  lexInit(&lexState, syntax, inputString);
  Object* obj = p_object(&lexState);
  if (lexState.error && obj != NULL) {
    obj->type = O_UNKNOWN;
  }
  return obj;
}

/* primitive 'spot' parser =========================================*/

Object* p_spot(LexerState* lexState, TokenType tt, Token* token) {
  /* save lexer state values in case the next token doesn't match */
  int pos = lexState->pos;
  int line = lexState->line;
  int col = lexState->col;
  /* try the next token */
  bool res = lexToken(lexState, token);
  if (res && token->type == tt) {
    Object* obj = malloc(sizeof(Object));
    obj->type = O_UNKNOWN;
    obj->s = token->lexeme;
    return obj;
  }
  /* no match, so restore the lexer state so that the lexer will
     return the same token again */
  lexState->pos = pos;
  lexState->line = line;
  lexState->col = col;
  return NULL;
}

/* parser combinators ==============================================*/

Object* p_maybe(LexerState* lexState, Parser* parser) {
  Object* obj = (*parser)(lexState);
  if (!obj) {
    obj = NOTHING;
  }
  return obj;
}

Object* p_oneOf(LexerState* lexState, Parser* parsers) {
  while (*parsers) {
    Object* obj = (*parsers)(lexState);
    if (lexState->error) {
      break;
    }
    if (obj) {
      return obj;
    }
    parsers++;
  }
  return NULL;
}

Object* p_some(LexerState* lexState, Parser* parser, int min) {
  Object* objs = EMPTY_LIST;
  while (true) {
    Object* obj = (*parser)(lexState);
    if (!obj)
      break;
    objs = listNew(obj, objs);
    min--;
  }
  if (min <= 0) {
    return objs;
  }
  /* FREE THE OBJECTS */
  return NULL;
}

/* Object parsers ==================================================*/

Object* p_int(LexerState* lexState) {
  Token token;
  Object* obj = p_spot(lexState, T_INT, &token);
  if (obj) {
    obj->type = O_INT;
    obj->i = atoi(token.lexeme);
  }
  return obj;
}

Object* p_real(LexerState* lexState) {
  Token token;
  Object* obj = p_spot(lexState, T_REAL, &token);
  if (obj) {
    obj->type = O_REAL;
    obj->f = atof(token.lexeme);
  }
  return obj;
}

Object* p_number(LexerState* lexState) {
  Parser parsers[] = {&p_int, &p_real, NULL};
  Object* obj = p_oneOf(lexState, parsers);
  return obj;
}

Object* p_string(LexerState* lexState) {
  Token token;
  Object* obj = p_spot(lexState, T_STRING, &token);
  if (obj) {
    obj->type = O_STRING;
    obj->s = malloc(strlen(token.lexeme) + 1);
    strcpy(obj->s, token.lexeme);
  }
  return obj;
}

Object* p_object(LexerState* lexState) {
  Parser parsers[] = {&p_int, &p_real, &p_string, NULL};
  Object* obj = p_oneOf(lexState, parsers);
  return obj;
}
#endif
