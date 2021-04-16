#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "d_array.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_string.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_if.h"
#include "globals.h"
#include "lexer_obj.h"
#include "parser.h"

Object p_ELSE(Object);
Object p_END(Object);
Object p_IF(Object);
Object p_THEN(Object);
Object p_expr(Object);

struct ParserEntry_struct {
  void* parserAddr;
  char* parserName;
} parserEntry[] = {
  {(void*)p_spot, "p_spot"},
  {(void*)p_spotReserved, "p_spotReserved"},
  {(void*)p_spotSpecial, "p_spotSpecial"},
  {(void*)p_ignore, "p_ignore"},
  {(void*)p_maybe, "p_maybe"},
  {(void*)p_oneOf, "p_oneOf"},
  {(void*)p_seq, "p_seq"},
  {(void*)p_some, "p_some"},
  {(void*)p_bool, "p_bool"},
  {(void*)p_int, "p_int"},
  {(void*)p_real, "p_real"},
  {(void*)p_string, "p_string"},
  {(void*)p_symbol, "p_symbol"},
  {(void*)p_number, "p_number"},
  {(void*)p_object, "p_object"},
  {(void*)p_ELSE, "p_ELSE"},
  {(void*)p_END, "p_END"},
  {(void*)p_IF, "p_IF"},
  {(void*)p_THEN, "p_THEN"},
  {(void*)p_ident, "p_ident"},
  {(void*)p_any, "p_any"},
  {(void*)p_if, "p_if"},
  {(void*)p_expr, "p_expr"},
  {0, 0}
};

char* lookupParserName(Parser parser) {
  struct ParserEntry_struct* pe = parserEntry;
  while (pe->parserAddr) {
    if (pe->parserAddr == (void*)parser) {
      return pe->parserName;
    }
    pe++;
  }
  return "unknown-parser";
}

/* Parser entry function */
Object parseCharString(char* input, Parser parser) {
  Object inputStr = stringNew(input);
  Object tokenQ = lex(inputStr);
  Object tokens = queueAsList(tokenQ);
  Object res = parse(parser, tokens);
  return res;
}

Object parse(Parser parser, Object tokens) {
#define DEBUG_PARSE 0
#if DEBUG_PARSE
  printf(" / parser.parse parser = %s (%p)\n", lookupParserName(parser), parser);
  printf(" > tokens = "); objShow(tokens, stdout); printf("\n");
#endif
  Object res = parser(tokens);
#if DEBUG_PARSE
  printf(" \\ parser.parse res = "); objShow(res, stdout); printf("\n");
#endif
  return res;
}

/* Primitive 'spot' parsers ----------------------------------------*/

Object p_spot(Object tokenList, TokenType tokenType) {
  Object token = listGetFirst(tokenList);
  Object tokenSym = arrayGet(token, 0);
  if (!symbolHasName(tokenSym, T_NAMES[tokenType])) {
    return nullObj;
  }
  return tokenList;
}

Object p_spotReserved(Object tokenList, char* word) {
  Object token = listGetFirst(tokenList);
  Object tokenSym = arrayGet(token, 0);
  if (!symbolHasName(tokenSym, T_NAMES[T_RESERVED])) {
    return nullObj;
  }
  Object tokenStr = arrayGet(token, 1);
  if (!stringEqualsChars(tokenStr, word)) {
    return nullObj;
  }
  return tokenList;
}

Object p_spotSpecial(Object tokenList, char* word) {
  Object token = listGetFirst(tokenList);
  Object tokenSym = arrayGet(token, 0);
  if (!symbolHasName(tokenSym, T_NAMES[T_SPECIAL])) {
    return nullObj;
  }
  Object tokenStr = arrayGet(token, 1);
  if (!stringEqualsChars(tokenStr, word)) {
    return nullObj;
  }
  return tokenList;
}

/* Parser combinators ----------------------------------------------*/

Object _ignore(Object res) {
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object tokens = listGetRest(res);
  return listNew(NOTHING, tokens);
}

Object p_ignore(Object tokens, Parser parser) {
  return _ignore(parse(parser, tokens));
}

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

Object p_seq(Object tokens, Parser* parsers) {
  Object objQ = queueNew();
  while (*parsers) {
    Object res = parse(*parsers, tokens);
    if (res.a == nullObj.a) {
      return nullObj;
    }
    Object obj = listGetFirst(res);
    if (obj.a != NOTHING.a) {
      queueEnq(objQ, listGetFirst(res));
    }
    tokens = listGetRest(tokens);
    parsers++;
  }
  Object objs = queueAsList(objQ);
  if (listGetRest(objs).a == EMPTY_LIST.a) {
    objs = listGetFirst(objs);
  }
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
  Object res = p_spot(tokens, T_BOOL);
  return res;
}

Object p_int(Object tokens) {
  Object res = p_spot(tokens, T_INT);
  return res;
}

Object p_real(Object tokens) {
  Object res = p_spot(tokens, T_REAL);
  return res;
}

Object p_string(Object tokens) {
  Object res = p_spot(tokens, T_STRING);
  return res;
}

Object p_symbol(Object tokens) {
  Object res = p_spot(tokens, T_SYMBOL);
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
Object p_ELSE(Object tokens) { return _ignore(p_spotReserved(tokens, "else")); }
Object p_END(Object tokens) { return _ignore(p_spotReserved(tokens, "end")); }
Object p_IF(Object tokens) { return _ignore(p_spotReserved(tokens, "if")); }
Object p_THEN(Object tokens) { return _ignore(p_spotReserved(tokens, "then")); }

Object p_ident(Object tokens) {
  Object res = p_spot(tokens, T_IDENT);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object resObj = listGetFirst(res);
  Object ident = arrayGet(resObj, 1);
  tokens = listGetRest(res);
  return listNew(ident, tokens);
}

Object p_if(Object tokens) {
  Parser parsers[] = {p_IF, p_any, p_THEN, p_any, p_ELSE, p_any, p_END, NULL};
  Object res = p_seq(tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object resList = listGetFirst(res);
  Object cond = listGetFirst(resList);
  Object conseq = listGetFirst(listGetRest(resList));
  Object alt = listGetFirst(listGetRest(listGetRest(resList)));
  Object ifExpr = ifNew(cond, conseq, alt);
  tokens = listGetRest(res);
  return listNew(ifExpr, tokens);
}

Object p_expr(Object tokens) {
  Parser parsers[] = {p_if, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}

Object p_any(Object tokens) {
  Parser parsers[] = {p_ident, p_int, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}
