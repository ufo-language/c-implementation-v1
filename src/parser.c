#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "d_array.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_symbol.h"
#include "delegate.h"
#include "globals.h"
#include "lexer.h"
#include "parser.h"

/* parser entry function */
#if 0
Object parse(char* inputString, Transition** syntax) {
  LexerState lexState;
  lexInit(&lexState, syntax, inputString);
  Object obj = p_object(&lexState);
  if (lexState.error && obj != NULL) {
    obj->type = O_UNKNOWN;
  }
  return obj;
}
#endif

/* primitive 'spot' parser =========================================*/

Object p_spot(Object tokenList, TokenType tokenType, Object value) {
  Object token = listGetFirst(tokenList);
  Object tokenSym = arrayGet(token, 0);
  if (!symbolHasName(tokenSym, T_NAMES[tokenType])) {
    return nullObj;
  }
  if (value.a != nullObj.a) {
    if (!objEquals(arrayGet(token, 1), value)) {
      return nullObj;
    }
  }
  return tokenList;
}

/* parser combinators ==============================================*/

Object p_maybe(Object tokens, Parser parser) {
  Object res = (parser)(tokens);
  return (res.a == nullObj.a) ? listNew(NOTHING, tokens) : res;
}

Object p_oneOf(Object tokens, Parser* parsers) {
  while (*parsers) {
    Object res = (*parsers)(tokens);
    if (res.a != nullObj.a) {
      return res;
    }
    parsers++;
  }
  return nullObj;
}

Object p_seq(Object tokens, Parser* parsers) {
  Object objQ = queueNew();
  while (*parsers) {
    Object res = (*parsers)(tokens);
    if (res.a == nullObj.a) {
      return nullObj;
    }
    queueEnq(objQ, listGetFirst(res));
    tokens = listGetRest(tokens);
    parsers++;
  }
  Object objs = queueAsList(objQ);
  return listNew(objs, tokens);
}

Object p_some(Object tokens, Parser parser, int min) {
  Object objQ = queueNew();
  while (true) {
    Object res = (*parser)(tokens);
    if (res.a == nullObj.a) {
      break;
    }
    Object obj = listGetFirst(res);
    queueEnq(objQ, obj);
    min--;
    tokens = listGetRest(tokens);
  }
  if (min <= 0) {
    Object objs = queueAsList(objQ);
    return listNew(objs, tokens);
  }
  return nullObj;
}

/* Object parsers ==================================================*/

Object p_int(Object tokens) {
  Object res = p_spot(tokens, T_INT, nullObj);
  return res;
}

Object p_real(Object tokens) {
  Object res = p_spot(tokens, T_REAL, nullObj);
  return res;
}

Object p_number(Object tokens) {
  Parser parsers[] = {&p_int, &p_real, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}

Object p_string(Object tokens) {
  Object res = p_spot(tokens, T_STRING, nullObj);
  return res;
}

Object p_object(Object tokens) {
  Parser parsers[] = {&p_int, &p_real, &p_string, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}
