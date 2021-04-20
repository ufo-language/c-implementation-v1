#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "d_array.h"
#include "d_binding.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_string.h"
#include "d_symbol.h"
#include "delegate.h"
#include "e_if.h"
#include "e_let.h"
#include "e_seq.h"
#include "globals.h"
#include "lexer_obj.h"
#include "parser.h"


#define DEBUG_PARSE 0


/* primitives */
Object p_spot(Object tokens, TokenType tokenType);
Object p_spotSpecific(Object tokens, TokenType tokenType, char* word);
Object p_spotOperator(Object tokens, char* word);
Object p_spotReserved(Object tokens, char* word);
Object p_spotSpecial(Object tokens, char* word);

/* combinators */
Object p_ignore(Thread* thd, Object tokens, Parser parser);
Object p_maybe(Thread* thd, Object tokens, Parser parser);
Object p_oneOf(Thread* thd, Object tokens, Parser* parsers);
Object p_sepBy(Thread* thd, Object tokens, Parser parser, Parser separator);
Object p_seq(Thread* thd, Object tokens, Parser* parsers);
Object p_some(Thread* thd, Object tokens, Parser parser, int min);

/* literals */
Object p_bool(Thread* thd, Object tokens);
Object p_int(Thread* thd, Object tokens);
Object p_real(Thread* thd, Object tokens);
Object p_string(Thread* thd, Object tokens);
Object p_symbol(Thread* thd, Object tokens);

Object p_literal(Thread* thd, Object tokens);

/* punctuation */
Object p_bar(Thread* thd, Object tokens);
Object p_braceOpen(Thread* thd, Object tokens);
Object p_braceClose(Thread* thd, Object tokens);
Object p_bracketOpen(Thread* thd, Object tokens);
Object p_bracketClose(Thread* thd, Object tokens);
Object p_comma(Thread* thd, Object tokens);
Object p_equalSign(Thread* thd, Object tokens);

/* containers */
Object p_array(Thread* thd, Object tokens);
Object p_binding(Thread* thd, Object tokens);
Object p_list(Thread* thd, Object tokens);

/* container & expression support */
Object p_commaBindings(Thread* thd, Object tokens);
Object p_commaAny(Thread* thd, Object tokens);
Object p_pattern(Thread* thd, Object tokens);

Object p_object(Thread* thd, Object tokens);

/* reserved words */
Object p_DO(Thread* thd, Object tokens);
Object p_ELSE(Thread* thd, Object tokens);
Object p_END(Thread* thd, Object tokens);
Object p_IF(Thread* thd, Object tokens);
Object p_LET(Thread* thd, Object tokens);
Object p_THEN(Thread* thd, Object tokens);

/* expressions */
Object p_do(Thread* thd, Object tokens);
Object p_ident(Thread* thd, Object tokens);
Object p_if(Thread* thd, Object tokens);
Object p_let(Thread* thd, Object tokens);

Object p_expr(Thread* thd, Object tokens);
Object p_listOfAny(Thread* thd, Object tokens);
Object p_any(Thread* thd, Object tokens);

struct ParserEntry_struct {
  void* parserAddr;
  char* parserName;
} parserEntry[] = {
  {(void*)p_DO, "p_DO"},
  {(void*)p_ELSE, "p_ELSE"},
  {(void*)p_END, "p_END"},
  {(void*)p_IF, "p_IF"},
  {(void*)p_LET, "p_LET"},
  {(void*)p_THEN, "p_THEN"},
  {(void*)p_any, "p_any"},
  {(void*)p_array, "p_array"},
  {(void*)p_binding, "p_binding"},
  {(void*)p_bool, "p_bool"},
  {(void*)p_bar, "p_bar"},
  {(void*)p_braceClose, "p_braceClose"},
  {(void*)p_braceOpen, "p_braceOpen"},
  {(void*)p_bracketClose, "p_bracketClose"},
  {(void*)p_bracketOpen, "p_bracketOpen"},
  {(void*)p_comma, "p_comma"},
  {(void*)p_commaAny, "p_commaAny"},
  {(void*)p_commaBindings, "p_commaBindings"},
  {(void*)p_do, "p_do"},
  {(void*)p_equalSign, "p_equalSign"},
  {(void*)p_expr, "p_expr"},
  {(void*)p_ident, "p_ident"},
  {(void*)p_if, "p_if"},
  {(void*)p_ignore, "p_ignore"},
  {(void*)p_int, "p_int"},
  {(void*)p_let, "p_let"},
  {(void*)p_let, "p_let"},
  {(void*)p_list, "p_list"},
  {(void*)p_listOfAny, "p_listOfAny"},
  {(void*)p_literal, "p_literal"},
  {(void*)p_maybe, "p_maybe"},
  {(void*)p_object, "p_object"},
  {(void*)p_oneOf, "p_oneOf"},
  {(void*)p_pattern, "p_pattern"},
  {(void*)p_real, "p_real"},
  {(void*)p_sepBy, "p_sepBy"},
  {(void*)p_seq, "p_seq"},
  {(void*)p_some, "p_some"},
  {(void*)p_spot, "p_spot"},
  {(void*)p_spotOperator, "p_spotOperator"},
  {(void*)p_spotReserved, "p_spotReserved"},
  {(void*)p_spotSpecial, "p_spotSpecial"},
  {(void*)p_spotSpecific, "p_spotSpecific"},
  {(void*)p_string, "p_string"},
  {(void*)p_symbol, "p_symbol"},
  {NULL, NULL}
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

#if DEBUG_PARSE
int debugLevel = 0;
void indent() {
  for (int n=0; n<debugLevel; n++) {
    printf("| ");
  }
}
#endif

/* parser entry point */
Object parseEntry(Thread* thd, Parser parser, Object tokens) {
  // make a copy of the thread's jumpBuf
  jmp_buf jumpBuf;
  memcpy(jumpBuf, thd->jumpBuf, sizeof(jumpBuf));
  // jump back here on error
  int jumpRes = setjmp(thd->jumpBuf);
  Object res = nullObj;
  switch (jumpRes) {
    case 0:
      // attempt a parse
      res = parse(thd, parser, tokens);
      break;
    case 1:
      printf("parseEntry caught parse error\n");
      res = nullObj;
      break;
  }
  // restore the thread's jumpbuf
  memcpy(thd->jumpBuf, jumpBuf, sizeof(jumpBuf));
  return res;
}

/* parse unbrella function, callable recursively */
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
  printf("\\ parser.parse %s res = ", lookupParserName(parser)); objShow(res, stdout); printf("\n");
#endif
  return res;
}

/* Utility functions -----------------------------------------------*/

/* Returns `nothing` in place of the last successful token. This is
   useful because the p_seq parser does not accumulate `nothing`
   tokens. */
static Object _ignore(Object parseRes) {
  if (parseRes.a == nullObj.a) {
    return nullObj;
  }
  Object tokens = listGetRest(parseRes);
  return listNew(NOTHING, tokens);
}

/* Extracts the token payload out of the token. */
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

Object p_spot(Object tokens, TokenType tokenType) {
  Object token = listGetFirst(tokens);
  Object tokenSym = arrayGet(token, 0);
  if (!symbolHasName(tokenSym, T_NAMES[tokenType])) {
    return nullObj;
  }
  return tokens;
}

Object p_spotSpecific(Object tokens, TokenType tokenType, char* word) {
  Object token = listGetFirst(tokens);
  Object tokenSym = arrayGet(token, 0);
  if (!symbolHasName(tokenSym, T_NAMES[tokenType])) {
    return nullObj;
  }
  Object tokenStr = arrayGet(token, 1);
  if (!stringEqualsChars(tokenStr, word)) {
    return nullObj;
  }
  return _ignore(tokens);
}

Object p_spotOperator(Object tokens, char* word) {
  return p_spotSpecific(tokens, T_OPER, word);
}

Object p_spotReserved(Object tokens, char* word) {
  return p_spotSpecific(tokens, T_RESERVED, word);
}

Object p_spotSpecial(Object tokens, char* word) {
  return p_spotSpecific(tokens, T_SPECIAL, word);
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
  Object lastObj = nullObj;
  bool firstIter = true;
  while (true) {
    /* parse an object */
    Object res = parse(thd, parser, tokens);
    if (res.a == nullObj.a) {
      if (firstIter) {
        break;
      }
      else {
        Object exnPayload = stringNew("PARSE ERROR: object expected after separator");
        threadThrowException(thd, exnPayload);
        /*
        fprintf(stderr, "ERROR: (parser) separator expected after object: ");
        objShow(tokens, stderr);
        fprintf(stderr, "\n");
        fprintf(stderr, "(change this to a longjmp instead of an exit(1)\n");
        exit(1);
        */
        return nullObj;
      }
    }
    Object obj = listGetFirst(res);
    lastObj = obj;
    queueEnq(objQ, obj);
    tokens = listGetRest(res);
    /* parse a separator */
    res = parse(thd, separator, tokens);
    if (res.a == nullObj.a) {
      break;
    }
    tokens = listGetRest(tokens);
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

/* literals */
Object p_bool(Thread* thd, Object tokens) {
  (void)thd;
  Object res = p_spot(tokens, T_BOOL);
  return _extract(res);
}

Object p_int(Thread* thd, Object tokens) {
  (void)thd;
  Object res = p_spot(tokens, T_INT);
  return _extract(res);
}

Object p_real(Thread* thd, Object tokens) {
  (void)thd;
  Object res = p_spot(tokens, T_REAL);
  return _extract(res);
}

Object p_string(Thread* thd, Object tokens) {
  (void)thd;
  Object res = p_spot(tokens, T_STRING);
  return _extract(res);
}

Object p_symbol(Thread* thd, Object tokens) {
  (void)thd;
  Object res = p_spot(tokens, T_SYMBOL);
  return _extract(res);
}

Object p_literal(Thread* thd, Object tokens) {
  Parser parsers[] = {p_bool, p_int, p_real, p_string, p_symbol, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}

/* punctuation */
Object p_bar(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "|");
}

Object p_braceOpen(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "{");
}

Object p_braceClose(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "}");
}

Object p_bracketOpen(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "[");
}

Object p_bracketClose(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "]");
}

Object p_equalSign(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotOperator(tokens, "=");
}

Object p_comma(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, ",");
}

/* containers */
Object p_commaAny(Thread* thd, Object tokens) {
  (void)thd;
  return p_sepBy(thd, tokens, p_any, p_comma);
}

Object p_array(Thread* thd, Object tokens) {
  Parser parsers[] = {p_braceOpen, p_commaAny, p_braceClose, NULL};
  Object res = p_seq(thd, tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object elems = listGetFirst(res);
  Word nElems = listCount(elems);
  Object ary = arrayNew(nElems);
  for (Word n=0; n<nElems; n++) {
    arraySet(ary, n, listGetFirst(elems));
    elems = listGetRest(elems);
  }
  tokens = listGetRest(res);
  return listNew(ary, tokens);
}

Object p_barAny(Thread* thd, Object tokens) {
  Parser parsers[] = {p_bar, p_any, NULL};
  return p_seq(thd, tokens, parsers);
}


Object p_list(Thread* thd, Object tokens) {
  Object res = p_bracketOpen(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  res = p_commaAny(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object elems = listGetFirst(res);
  tokens = listGetRest(res);
  res = p_barAny(thd, tokens);
  if (res.a != nullObj.a) {
    Object impropElem = listGetFirst(res);
    tokens = listGetRest(res);
    /* rebuild list with `improp` as the final improper element */
    Object pair = listGetLastPair(elems);
    listSetRest(pair, impropElem);
  }
  res = p_bracketClose(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  return listNew(elems, tokens);
}

/* a pattern is the left-hand-side of a binding */
Object p_pattern(Thread* thd, Object tokens) {
  Parser parsers[] = {p_ident, p_literal, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}

Object p_binding(Thread* thd, Object tokens) {
  Parser parsers[] = {p_pattern, p_equalSign, p_any, NULL};
  Object res = p_seq(thd, tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object resObj = listGetFirst(res);
  Object lhs = listGetFirst(resObj);
  Object rhs = listGetSecond(resObj);
  Object bnd = bindingNew(lhs, rhs);
  tokens = listGetRest(res);
  return listNew(bnd, tokens);
}

Object p_object(Thread* thd, Object tokens) {
  Parser parsers[] = {p_array, p_list, p_binding, p_literal, p_ident, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}

/* Expression parsers ----------------------------------------------*/

/* reserved words */
Object p_DO(Thread* thd, Object tokens) {
  (void)thd;
  return _ignore(p_spotReserved(tokens, "do"));
}

Object p_ELSE(Thread* thd, Object tokens) {
  (void)thd;
  return _ignore(p_spotReserved(tokens, "else"));
}

Object p_END(Thread* thd, Object tokens) {
  (void)thd;
  return _ignore(p_spotReserved(tokens, "end"));
}

Object p_IF(Thread* thd, Object tokens) {
  (void)thd;
  return _ignore(p_spotReserved(tokens, "if"));
}

Object p_LET(Thread* thd, Object tokens) {
  (void)thd;
  return _ignore(p_spotReserved(tokens, "let"));
}

Object p_THEN(Thread* thd, Object tokens) {
  (void)thd;
  return _ignore(p_spotReserved(tokens, "then"));
}

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
  (void)thd;
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

Object p_commaBindings(Thread* thd, Object tokens) {
  return p_sepBy(thd, tokens, p_binding, p_comma);
}

Object p_let(Thread* thd, Object tokens) {
  Parser parsers[] = {p_LET, p_commaBindings, NULL};
  Object res = p_seq(thd, tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object bindings = listGetFirst(res);
  tokens = listGetRest(res);
  Object let = letNew(bindings);
  return listNew(let, tokens);
}

/* any expression */
Object p_expr(Thread* thd, Object tokens) {
  Parser parsers[] = {p_do, p_if, p_let, NULL};
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
  Parser parsers[] = {p_expr, p_object, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}
