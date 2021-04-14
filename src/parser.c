#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "d_array.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_string.h"
#include "d_symbol.h"
#include "delegate.h"
#include "globals.h"
#include "lexer_obj.h"
#include "parser.h"

/* Parser entry function */
Object parseCharString(char* input, Parser parser) {
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = parse(parser, tokens);
  return res;
}

Object parse(Parser parser, Object tokens) {
  Object res = parser(tokens);
  return res;
}

/* Primitive 'spot' parser -----------------------------------------*/

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

Object p_spotReserved(Object tokenList, char* word) {
  Object token = listGetFirst(tokenList);
  Object tokenSym = arrayGet(token, 0);
  if (!symbolHasName(tokenSym, T_NAMES[T_RESERVED])) {
    return nullObj;
  }
  Object string = arrayGet(token, 1);
  return tokenList;
}

/* Parser combinators ----------------------------------------------*/

Object p_maybe(Object tokens, Parser parser) {
  Object res = parse(parser, tokens);
  return (res.a == nullObj.a) ? listNew(NOTHING, tokens) : res;
}

Object p_oneOf(Object tokens, Parser* parsers) {
  while (*parsers) {
    Object res = parse(*parsers, tokens);
    if (res.a != nullObj.a) {
      return res;
    }
    parsers++;
  }
  return nullObj;
}

Object p_reserved(Object tokens, Object reservedString) {
  Object res = p_spot(tokens, T_RESERVED, reservedString);
  return res;
}

Object p_seq(Object tokens, Parser* parsers) {
  Object objQ = queueNew();
  while (*parsers) {
    Object res = parse(*parsers, tokens);
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
    Object res = parse(parser, tokens);
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

/* Object parsers --------------------------------------------------*/

Object p_bool(Object tokens) {
  Object res = p_spot(tokens, T_BOOL, nullObj);
  return res;
}

Object p_int(Object tokens) {
  Object res = p_spot(tokens, T_INT, nullObj);
  return res;
}

Object p_real(Object tokens) {
  Object res = p_spot(tokens, T_REAL, nullObj);
  return res;
}

Object p_string(Object tokens) {
  Object res = p_spot(tokens, T_STRING, nullObj);
  return res;
}

Object p_symbol(Object tokens) {
  Object res = p_spot(tokens, T_SYMBOL, nullObj);
  return res;
}

/* Aggregate object parsers ----------------------------------------*/

Object p_number(Object tokens) {
  Parser parsers[] = {p_int, p_real, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}

Object p_object(Object tokens) {
  Parser parsers[] = {p_int, p_bool, p_symbol, p_string, p_real, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}

/* Expression parsers ----------------------------------------------*/

/* reserved words */
Object p_end(Object tokens) {
  return p_spotReserved(tokens, "end");
}

Object p_ident(Object tokens) {
  Object res = p_spot(tokens, T_IDENT, nullObj);
  return res;
}

Object p_if(Object tokens) {
  return p_spotReserved(tokens, "if");
}
