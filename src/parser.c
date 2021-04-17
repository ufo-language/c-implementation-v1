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

/* combinators */
Object p_sepBy(Thread* thd, Object tokens, Parser parser, Parser separator);

/* objects */
Object p_binding(Thread* thd, Object tokens);

/* reserved words */
Object p_DO(Thread* thd, Object tokens);
Object p_ELSE(Thread* thd, Object tokens);
Object p_END(Thread* thd, Object tokens);
Object p_IF(Thread* thd, Object tokens);
Object p_LET(Thread* thd, Object tokens);
Object p_THEN(Thread* thd, Object tokens);

/* expressions */
Object p_do(Thread* thd, Object tokens);
Object p_if(Thread* thd, Object tokens);
Object p_let(Thread* thd, Object tokens);

Object p_expr(Thread* thd, Object tokens);
Object p_listOfAny(Thread* thd, Object tokens);

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

Object parse(Thread* thd, Parser parser, Object tokens) {
#if DEBUG_PARSE
  indent(); 
  printf("/ parser.parse parser = %s (%p)\n", lookupParserName(parser), (void*)parser);
  indent();
  printf("> tokens = "); objShow(tokens, stdout); printf("\n");
  debugLevel++;
#endif
  Object res = parser(thd, tokens);
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

Object p_ignore(Thread* thd, Object tokens, Parser parser) {
  return _ignore(parse(thd, parser, tokens));
}

Object p_maybe(Thread* thd, Object tokens, Parser parser) {
  Object res = parse(thd, parser, tokens);
  return (res.a == nullObj.a) ? listNew(NOTHING, tokens) : res;
}

Object p_oneOf(Thread* thd, Object tokens, Parser* parsers) {
  while (*parsers) {
    Object res = parse(thd, *parsers, tokens);
    if (res.a != nullObj.a) {
      return res;
    }
    parsers++;
  }
  return nullObj;
}

Object p_sepBy(Thread* thd, Object tokens, Parser parser, Parser separator) {
  Object objQ = queueNew();
  bool firstIter = true;
  while (true) {
    /* parse an object */
    Object res = parse(thd, parser, tokens);
    if (res.a == nullObj.a) {
      if (firstIter) {
        break;
      }
      else {
        fprintf(stderr, "separator expected after object: ");
        objShow(tokens, stderr);
        printf("\n");
        return nullObj;
      }
    }
    Object obj = listGetFirst(res);
    queueEnq(objQ, obj);
    tokens = listGetRest(tokens);
    /* parse a separator */
    /* TODO finish this*/
    firstIter = false;
  }
  Object objs = queueAsList(objQ);
  return listNew(objs, tokens);
}

Object p_seq(Thread* thd, Object tokens, Parser* parsers) {
  Object objQ = queueNew();
  while (*parsers) {
    Object res = parse(thd, *parsers, tokens);
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

Object p_some(Thread* thd, Object tokens, Parser parser, int min) {
  Object objQ = queueNew();
  while (true) {
    Object res = parse(thd, parser, tokens);
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

Object p_bool(Thread* thd, Object tokens) {
  Object res = p_spot(tokens, T_BOOL);
  return res;
}

/* support for p_binding */
Object p_pattern(Thread* thd, Object tokens) {
}

/* support for p_binding */
Object p_equalSign(Thread* thd, Object tokens) {
  return p_spotSpecial(tokens, "=");
}

Object p_binding(Thread* thd, Object tokens) {
  Parser parser[] = {p_pattern, p_equalSign, p_any, NULL};
}

Object p_int(Thread* thd, Object tokens) {
  Object res = p_spot(tokens, T_INT);
  return res;
}

Object p_real(Thread* thd, Object tokens) {
  Object res = p_spot(tokens, T_REAL);
  return res;
}

Object p_string(Thread* thd, Object tokens) {
  Object res = p_spot(tokens, T_STRING);
  return res;
}

Object p_symbol(Thread* thd, Object tokens) {
  Object res = p_spot(tokens, T_SYMBOL);
  return res;
}

/* Aggregate object parsers ----------------------------------------*/

/*Object p_number(Thread* thd, Object tokens) {
  Parser parsers[] = {p_int, p_real, NULL};
  Object res = p_oneOf(tokens, parsers);
  return res;
}*/

Object p_object(Thread* thd, Object tokens) {
  Parser parsers[] = {p_int, p_bool, p_symbol, p_string, p_real, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return _extract(res);
}

/* Expression parsers ----------------------------------------------*/

/* reserved words */
Object p_DO(Thread* thd, Object tokens) { return _ignore(p_spotReserved(tokens, "do")); }
Object p_ELSE(Thread* thd, Object tokens) { return _ignore(p_spotReserved(tokens, "else")); }
Object p_END(Thread* thd, Object tokens) { return _ignore(p_spotReserved(tokens, "end")); }
Object p_IF(Thread* thd, Object tokens) { return _ignore(p_spotReserved(tokens, "if")); }
Object p_LET(Thread* thd, Object tokens) { return _ignore(p_spotReserved(tokens, "let")); }
Object p_THEN(Thread* thd, Object tokens) { return _ignore(p_spotReserved(tokens, "then")); }

Object p_do(Thread* thd, Object tokens) {
  Parser parsers[] = {p_DO, p_listOfAny, p_END, NULL};
  Object res = p_seq(thd, tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object exprs = listGetFirst(res);
  Object doExpr = seqNew(exprs);
  tokens = listGetRest(res);
  return listNew(doExpr, tokens);
}

Object p_ident(Thread* thd, Object tokens) {
  Object res = p_spot(tokens, T_IDENT);
  return _extract(res);
}

Object p_if(Thread* thd, Object tokens) {
  Parser parsers[] = {p_IF, p_any, p_THEN, p_any, p_ELSE, p_any, p_END, NULL};
  Object res = p_seq(thd, tokens, parsers);
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

Object p_comma(Thread* thd, Object tokens) {
  return p_spotSpecial(tokens, ",");
}

Object p_commaBindings(Thread* thd, Object tokens) {
  return p_sepBy(thd, tokens, p_binding, p_comma);
}

Object p_let(Thread* thd, Object tokens) {
  Parser parsers[] = {p_LET, p_commaBindings, NULL};
}

/* any expression */
Object p_expr(Thread* thd, Object tokens) {
  Parser parsers[] = {p_do, p_if, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}

/* list of expressions */
Object p_listOfAny(Thread* thd, Object tokens) {
  Object res = p_some(thd, tokens, p_any, 0);
  return res;
}

/* any object or expression */
Object p_any(Thread* thd, Object tokens) {
  Parser parsers[] = {p_ident, p_object, p_if, p_do, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}
