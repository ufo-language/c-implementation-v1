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
#include "e_seq.h"
#include "globals.h"
#include "lexer_obj.h"
#include "parser.h"

Object p_DO(Object tokens);
Object p_ELSE(Object tokens);
Object p_END(Object tokens);
Object p_IF(Object tokens);
Object p_THEN(Object tokens);

Object p_do(Object tokens);
Object p_expr(Object tokens);
Object p_listOfAny(Object tokens);

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
  //{(void*)p_number, "p_number"},
  {(void*)p_object, "p_object"},
  {(void*)p_DO, "p_DO"},
  {(void*)p_ELSE, "p_ELSE"},
  {(void*)p_END, "p_END"},
  {(void*)p_IF, "p_IF"},
  {(void*)p_THEN, "p_THEN"},
  {(void*)p_ident, "p_ident"},
  {(void*)p_if, "p_if"},
  {(void*)p_do, "p_do"},
  {(void*)p_expr, "p_expr"},
  {(void*)p_any, "p_any"},
  {(void*)p_listOfAny, "p_listOfAny"},
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

#define DEBUG_PARSE 0

#if DEBUG_PARSE
int debugLevel = 0;
void indent() {
  for (int n=0; n<debugLevel; n++) {
    printf("| ");
  }
}
#endif

Object parse(Parser parser, Object tokens) {
#if DEBUG_PARSE
  indent(); 
  printf("/ parser.parse parser = %s (%p)\n", lookupParserName(parser), (void*)parser);
  indent();
  printf("> tokens = "); objShow(tokens, stdout); printf("\n");
  debugLevel++;
#endif
  Object res = parser(tokens);
#if DEBUG_PARSE
  debugLevel--;
  indent();
  printf("\\ parser.parse res = "); objShow(res, stdout); printf("\n");
#endif
  return res;
}

/* Utility functions -----------------------------------------------*/

/* Returns `nothing` in place of the last successful token. The p_seq
   parser does not accumulate `nothing` tokens. */
static Object _ignore(Object parseRes) {
  if (parseRes.a == nullObj.a) {
    return nullObj;
  }
  Object tokens = listGetRest(parseRes);
  return listNew(NOTHING, tokens);
}

/* Extracts the token payload out of the last successful token. */
static Object _extract(Object parseRes) {
  if (parseRes.a == nullObj.a) {
    return nullObj;
  }
  Object resObj = listGetFirst(parseRes);
  Object tokens = listGetRest(parseRes);
  Object ident = arrayGet(resObj, 1);
  tokens = listGetRest(parseRes);
  return listNew(ident, tokens);
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
    tokens = listGetRest(res);
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

/*Object p_number(Object tokens) {
  Parser parsers[] = {p_int, p_real, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}*/

Object p_object(Object tokens) {
  Parser parsers[] = {p_int, p_bool, p_symbol, p_string, p_real, NULL};
  Object res = p_oneOf(tokens, parsers);
  return _extract(res);
}

/* Expression parsers ----------------------------------------------*/

/* reserved words */
Object p_DO(Object tokens) { return _ignore(p_spotReserved(tokens, "do")); }
Object p_ELSE(Object tokens) { return _ignore(p_spotReserved(tokens, "else")); }
Object p_END(Object tokens) { return _ignore(p_spotReserved(tokens, "end")); }
Object p_IF(Object tokens) { return _ignore(p_spotReserved(tokens, "if")); }
Object p_THEN(Object tokens) { return _ignore(p_spotReserved(tokens, "then")); }

Object p_do(Object tokens) {
  Parser parsers[] = {p_DO, p_listOfAny, p_END, NULL};
  Object res = p_seq(tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object exprs = listGetFirst(res);
  Object doExpr = seqNew(exprs);
  tokens = listGetRest(res);
  return listNew(doExpr, tokens);
}

Object p_ident(Object tokens) {
  Object res = p_spot(tokens, T_IDENT);
  return _extract(res);
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

/* any expression */
Object p_expr(Object tokens) {
  Parser parsers[] = {p_do, p_if, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}

/* list of expressions */
Object p_listOfAny(Object tokens) {
  Object res = p_some(tokens, p_any, 0);
  return res;
}

/* any object or expression */
Object p_any(Object tokens) {
  Parser parsers[] = {p_ident, p_object, p_if, p_do, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}
