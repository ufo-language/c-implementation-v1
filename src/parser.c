#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "d_array.h"
#include "d_binding.h"
#include "d_hash.h"
#include "d_list.h"
#include "d_queue.h"
#include "d_set.h"
#include "d_string.h"
#include "d_symbol.h"
#include "d_tuple.h"
#include "delegate.h"
#include "e_abstr.h"
#include "e_app.h"
#include "e_if.h"
#include "e_let.h"
#include "e_letrec.h"
#include "e_quote.h"
#include "e_seq.h"
#include "globals.h"
#include "lexer_obj.h"
#include "parser.h"

#define DEBUG_PARSE 0

#define PARSER_ERROR "ParserError"

/* primitives */
Object p_spot(Object tokens, TokenType tokenType);
Object p_spotSpecific(Object tokens, TokenType tokenType, char* word);
Object p_spotOperator(Object tokens, char* word);
Object p_spotReserved(Object tokens, char* word);
Object p_spotReserved_required(Thread* thd, Object tokens, char* word);
Object p_spotSpecial(Object tokens, char* word);
Object p_spotSpecial_required(Thread* thd, Object tokens, char* word);

/* combinators */
Object p_fail(Thread* thd, Object tokens, char* message);
Object p_ignore(Thread* thd, Object tokens, Parser parser);
Object p_maybe(Thread* thd, Object tokens, Parser parser);
Object p_oneOf(Thread* thd, Object tokens, Parser* parsers);
Object p_sepBy(Thread* thd, Object tokens, Parser parser, Parser separator);
Object p_seqOf(Thread* thd, Object tokens, Parser* parsers);
Object p_some(Thread* thd, Object tokens, Parser parser, int min);

/* literals */
Object p_bool(Thread* thd, Object tokens);
Object p_int(Thread* thd, Object tokens);
Object p_real(Thread* thd, Object tokens);
Object p_string(Thread* thd, Object tokens);
Object p_symbol(Thread* thd, Object tokens);

Object p_literal(Thread* thd, Object tokens);

/* punctuation */
Object p_angleOpen(Thread* thd, Object tokens);
Object p_angleClose(Thread* thd, Object tokens);
Object p_bar(Thread* thd, Object tokens);
Object p_braceOpen(Thread* thd, Object tokens);
Object p_braceClose(Thread* thd, Object tokens);
Object p_bracketOpen(Thread* thd, Object tokens);
Object p_bracketClose(Thread* thd, Object tokens);
Object p_comma(Thread* thd, Object tokens);
Object p_equalSign(Thread* thd, Object tokens);
Object p_hashMark(Thread* thd, Object tokens);
Object p_parenOpen(Thread* thd, Object tokens);
Object p_parenClose(Thread* thd, Object tokens);
Object p_singleQuote(Thread* thd, Object tokens);

/* containers */
Object p_array(Thread* thd, Object tokens);
Object p_binding(Thread* thd, Object tokens);
Object p_hashTable(Thread* thd, Object tokens);
Object p_list(Thread* thd, Object tokens);
Object p_tuple(Thread* thd, Object tokens);

/* container & expression support */
Object p_commaBindings(Thread* thd, Object tokens);
Object p_commaList(Thread* thd, Object tokens);
Object p_funRule(Thread* thd, Object tokens);
Object p_parenCommaList(Thread* thd, Object tokens);
Object p_pattern(Thread* thd, Object tokens);

Object p_object(Thread* thd, Object tokens);

/* reserved words */
Object p_DO(Thread* thd, Object tokens);
Object p_ELSE(Thread* thd, Object tokens);
Object p_END(Thread* thd, Object tokens);
Object p_END_required(Thread* thd, Object tokens);
Object p_FUN(Thread* thd, Object tokens);
Object p_IF(Thread* thd, Object tokens);
Object p_LET(Thread* thd, Object tokens);
Object p_LETREC(Thread* thd, Object tokens);
Object p_THEN(Thread* thd, Object tokens);

/* expressions */
Object p_apply(Thread* thd, Object tokens);
Object p_do(Thread* thd, Object tokens);
Object p_function(Thread* thd, Object tokens);
Object p_ident(Thread* thd, Object tokens);
Object p_if(Thread* thd, Object tokens);
Object p_let(Thread* thd, Object tokens);
Object p_letRec(Thread* thd, Object tokens);
Object p_parenExpr(Thread* thd, Object tokens);
Object p_queue(Thread* thd, Object tokens);
Object p_quote(Thread* thd, Object tokens);
Object p_set(Thread* thd, Object tokens);

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
  {(void*)p_END_required, "p_END_required"},
  {(void*)p_FUN, "p_FUN"},
  {(void*)p_IF, "p_IF"},
  {(void*)p_LET, "p_LET"},
  {(void*)p_THEN, "p_THEN"},
  {(void*)p_angleClose, "p_angleClose"},
  {(void*)p_angleOpen, "p_angleOpen"},
  {(void*)p_any, "p_any"},
  {(void*)p_apply, "p_apply"},
  {(void*)p_array, "p_array"},
  {(void*)p_binding, "p_binding"},
  {(void*)p_bool, "p_bool"},
  {(void*)p_bar, "p_bar"},
  {(void*)p_braceClose, "p_braceClose"},
  {(void*)p_braceOpen, "p_braceOpen"},
  {(void*)p_bracketClose, "p_bracketClose"},
  {(void*)p_bracketOpen, "p_bracketOpen"},
  {(void*)p_comma, "p_comma"},
  {(void*)p_commaList, "p_commaList"},
  {(void*)p_commaBindings, "p_commaBindings"},
  {(void*)p_do, "p_do"},
  {(void*)p_equalSign, "p_equalSign"},
  {(void*)p_expr, "p_expr"},
  {(void*)p_fail, "p_fail"},
  {(void*)p_function, "p_function"},
  {(void*)p_funRule, "p_funRule"},
  {(void*)p_hashMark, "p_hashMark"},
  {(void*)p_hashTable, "p_hashTable"},
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
  {(void*)p_parenClose, "p_parenClose"},
  {(void*)p_parenCommaList, "p_parenCommaList"},
  {(void*)p_parenExpr, "p_parenExpr"},
  {(void*)p_parenOpen, "p_parenOpen"},
  {(void*)p_pattern, "p_pattern"},
  {(void*)p_quote, "p_quote"},
  {(void*)p_real, "p_real"},
  {(void*)p_sepBy, "p_sepBy"},
  {(void*)p_seqOf, "p_seqOf"},
  {(void*)p_set, "p_set"},
  {(void*)p_singleQuote, "p_singleQuote"},
  {(void*)p_some, "p_some"},
  {(void*)p_spot, "p_spot"},
  {(void*)p_spotOperator, "p_spotOperator"},
  {(void*)p_spotReserved, "p_spotReserved"},
  {(void*)p_spotReserved_required, "p_spotReserved_required"},
  {(void*)p_spotSpecial, "p_spotSpecial"},
  {(void*)p_spotSpecial_required, "p_spotSpecial_required"},
  {(void*)p_spotSpecific, "p_spotSpecific"},
  {(void*)p_string, "p_string"},
  {(void*)p_symbol, "p_symbol"},
  {(void*)p_tuple, "p_tuple"},
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
Object parseEntry(Thread* thd, Object tokens) {
  return parse(thd, p_any, tokens);
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
   useful because the p_seqOf parser does not accumulate `nothing`
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

Object p_spotOperator_required(Thread* thd, Object tokens, char* word) {
  Object res = p_spotSpecific(tokens, T_OPER, word);
  if (res.a == nullObj.a) {
    Object exn = arrayNew(4);
    arraySet(exn, 0, symbolNew(PARSER_ERROR));
    arraySet(exn, 1, stringNew("character expected"));
    arraySet(exn, 2, stringNew(word));
    arraySet(exn, 3, tokens);
    threadThrowExceptionObj(thd, exn);
  }
  return res;
}

Object p_spotReserved(Object tokens, char* word) {
  return p_spotSpecific(tokens, T_RESERVED, word);
}

Object p_spotReserved_required(Thread* thd, Object tokens, char* word) {
  Object res = p_spotSpecific(tokens, T_RESERVED, word);
  if (res.a == nullObj.a) {
    Object exn = arrayNew(3);
    arraySet(exn, 0, symbolNew(PARSER_ERROR));
    arraySet(exn, 1, stringNew("keyword expected"));
    arraySet(exn, 2, stringNew(word));
    threadThrowExceptionObj(thd, exn);
  }
  return res;
}

Object p_spotSpecial(Object tokens, char* word) {
  return p_spotSpecific(tokens, T_SPECIAL, word);
}

Object p_spotSpecial_required(Thread* thd, Object tokens, char* word) {
  Object res = p_spotSpecific(tokens, T_SPECIAL, word);
  if (res.a == nullObj.a) {
    Object exn = arrayNew(4);
    arraySet(exn, 0, symbolNew(PARSER_ERROR));
    arraySet(exn, 1, stringNew("character expected"));
    arraySet(exn, 2, stringNew(word));
    arraySet(exn, 3, tokens);
    threadThrowExceptionObj(thd, exn);
  }
  return res;
}

/* Parser combinators ----------------------------------------------*/

Object p_fail(Thread* thd, Object tokens, char* message) { 
 threadThrowException(thd, PARSER_ERROR, message, tokens);
 return nullObj;  /* suppress compiler warning */
}

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
        p_fail(thd, tokens, "object expected after separator");
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

Object p_seqOf(Thread* thd, Object tokens, Parser* parsers) {
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
    tokens = listGetRest(res);
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

Object p_angleOpen(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotOperator(tokens, "<");
}

Object p_angleClose(Thread* thd, Object tokens) {
  return p_spotOperator_required(thd, tokens, ">");
}

Object p_bar(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "|");
}

Object p_braceOpen(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "{");
}

Object p_braceClose(Thread* thd, Object tokens) {
  return p_spotSpecial_required(thd, tokens, "}");
}

Object p_bracketOpen(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "[");
}

Object p_bracketClose(Thread* thd, Object tokens) {
  return p_spotSpecial_required(thd, tokens, "]");
}

Object p_equalSign(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotOperator(tokens, "=");
}

Object p_comma(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, ",");
}

Object p_hashMark(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "#");
}

Object p_parenOpen(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "(");
}

Object p_parenClose(Thread* thd, Object tokens) {
  return p_spotSpecial_required(thd, tokens, ")");
}

Object p_singleQuote(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotSpecial(tokens, "'");
}

/* containers */
Object p_commaList(Thread* thd, Object tokens) {
  (void)thd;
  return p_sepBy(thd, tokens, p_any, p_comma);
}

Object p_array(Thread* thd, Object tokens) {
  Parser parsers[] = {p_braceOpen, p_commaList, p_braceClose, NULL};
  Object res = p_seqOf(thd, tokens, parsers);
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
  return p_seqOf(thd, tokens, parsers);
}


Object p_hashTable(Thread* thd, Object tokens) {
  Parser parsers[] = {p_hashMark, p_braceOpen, p_commaBindings, p_braceClose, NULL};
  Object res = p_seqOf(thd, tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object hash = hashNew();
  Object bindings = listGetFirst(res);
  while (!listIsEmpty(bindings)) {
    Object binding = listGetFirst(bindings);
    hashPut(hash, bindingGetLhs(binding), bindingGetRhs(binding));
    bindings = listGetRest(bindings);
  }
  tokens = listGetRest(res);
  return listNew(hash, tokens);
}

Object p_list(Thread* thd, Object tokens) {
  Object res = p_bracketOpen(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  res = p_commaList(thd, tokens);
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

Object p_tuple(Thread* thd, Object tokens) {
  Parser parsers[] = {p_angleOpen, p_commaList, p_angleClose, NULL};
  Object res = p_seqOf(thd, tokens, parsers);
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
  Object tuple = tupleFromArray(ary);
  return listNew(tuple, tokens);
}

/* a pattern is the left-hand-side of a binding */
Object p_pattern(Thread* thd, Object tokens) {
  Parser parsers[] = {p_ident, p_literal, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}

Object p_binding(Thread* thd, Object tokens) {
  Parser parsers[] = {p_pattern, p_equalSign, p_any, NULL};
  Object res = p_seqOf(thd, tokens, parsers);
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
  Parser parsers[] = {p_array, p_list, p_hashTable, p_queue, p_set, p_tuple, p_binding, p_literal, p_ident, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}

/* Expression parsers ----------------------------------------------*/

/* reserved words */
Object p_DO(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotReserved(tokens, "do");
}

Object p_ELSE(Thread* thd, Object tokens) {
  return p_spotReserved_required(thd, tokens, "else");
}

Object p_END(Thread* thd, Object tokens) {
  return p_spotReserved_required(thd, tokens, "end");
}

Object p_END_required(Thread* thd, Object tokens) {
  Object res = p_spotReserved(tokens, "end");
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "Keyword 'end' expected");
  }
  return _ignore(res);
}

Object p_FUN(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotReserved(tokens, "fun");
}

Object p_IF(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotReserved(tokens, "if");
}

Object p_LET(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotReserved(tokens, "let");
}

Object p_LETREC(Thread* thd, Object tokens) {
  (void)thd;
  return p_spotReserved(tokens, "letrec");
}

Object p_THEN(Thread* thd, Object tokens) {
  return p_spotReserved_required(thd, tokens, "then");
}

Object p_parenCommaList(Thread* thd, Object tokens) {
  Parser parsers[] = {p_parenOpen, p_commaList, p_parenClose, NULL};
  return p_seqOf(thd, tokens, parsers);
}

Object p_apply(Thread* thd, Object tokens) {
  /* try a parenthesized expression, and if that fails then try just
     an identifier */
  Object abstrRes = p_parenExpr(thd, tokens);
  if (abstrRes.a == nullObj.a) {
    abstrRes = p_ident(thd, tokens);
    if (abstrRes.a == nullObj.a) {
      return nullObj;
    }
  }
  Object abstr = listGetFirst(abstrRes);
  tokens = listGetRest(abstrRes);
  /* parse an argument list */
  Object argsRes = p_parenCommaList(thd, tokens);
  if (argsRes.a == nullObj.a) {
    return nullObj;
  }
  Object args = listGetFirst(argsRes);
  tokens = listGetRest(argsRes);
  Object apply = appNew(abstr, args);
  return listNew(apply, tokens);
}

Object p_do(Thread* thd, Object tokens) {
  Parser parsers[] = {p_DO, p_listOfAny, p_END_required, NULL};
  Object res = p_seqOf(thd, tokens, parsers);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  Object exprs = listGetFirst(res);
  Object doExpr = seqNew(exprs);
  tokens = listGetRest(res);
  return listNew(doExpr, tokens);
}

Object p_funRule(Thread* thd, Object tokens) {
  Parser parsers[] = {p_parenCommaList, p_equalSign, p_listOfAny, NULL};
  Object res = p_seqOf(thd, tokens, parsers);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "function rule expected");
  }
  return res;
}

Object p_function(Thread* thd, Object tokens) {
  Object res = p_FUN(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  Object funNameRes = p_ident(thd, tokens);
  Object funName = nullObj;
  if (funNameRes.a != nullObj.a) {
    funName = listGetFirst(funNameRes);
    tokens = listGetRest(funNameRes);
  }
  Object rulesRes = p_sepBy(thd, tokens, p_funRule, p_bar);
  Object rules = listGetFirst(rulesRes);
  if (listCount(rules) == 0) {
    p_fail(thd, tokens, "function rules expected");
  }
  tokens = listGetRest(rulesRes);
  res = p_END(thd, tokens);
  tokens = listGetRest(res);
  /* build the function object */
  Object fun = nullObj;
  Object prev = nullObj;
  while (!listIsEmpty(rules)) {
    Object rule = listGetFirst(rules);
    Object params = listGetFirst(rule);
    Object body = listGetSecond(rule);
    Object fun1 = abstrNew(params, body);
    if (fun.a == nullObj.a) {
      fun = fun1;
    }
    else {
      abstrSetNext(prev, fun1);
    }
    prev = fun1;
    rules = listGetRest(rules);
  }
  /* if the function has a name, convert it to a letrec expression */
  if (funName.a != nullObj.a) {
    Object binding = bindingNew(funName, fun);
    Object bindings = listNew(binding, EMPTY_LIST);
    fun = letRecNew(bindings);
  }
  return listNew(fun, tokens);
}

Object p_ident(Thread* thd, Object tokens) {
  (void)thd;
  Object res = p_spot(tokens, T_IDENT);
  return _extract(res);
}

Object p_if(Thread* thd, Object tokens) {
  Parser parsers[] = {p_IF, p_any, p_THEN, p_any, p_ELSE, p_any, p_END_required, NULL};
  Object res = p_seqOf(thd, tokens, parsers);
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
  Object res = p_LET(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  res = p_commaBindings(thd, tokens);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "bindings expected after 'let'");
  }
  Object bindings = listGetFirst(res);
  if (listCount(bindings) == 0) {
    p_fail(thd, tokens, "bindings expected after 'let'");
  }
  tokens = listGetRest(res);
  Object let = letNew(bindings);
  return listNew(let, tokens);
}

Object p_letRec(Thread* thd, Object tokens) {
  Object res = p_LETREC(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  res = p_commaBindings(thd, tokens);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "bindings expected after 'letrec'");
  }
  Object bindings = listGetFirst(res);
  if (listCount(bindings) == 0) {
    p_fail(thd, tokens, "bindings expected after 'letrec'");
  }
  tokens = listGetRest(res);
  Object let = letRecNew(bindings);
  return listNew(let, tokens);
}

Object p_parenExpr(Thread* thd, Object tokens) {
  Object openRes = p_parenOpen(thd, tokens);
  if (openRes.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(openRes);
  Object exprRes = p_expr(thd, tokens);
  if (exprRes.a == nullObj.a) {
    return nullObj;
  }
  Object expr = listGetFirst(exprRes);
  tokens = listGetRest(exprRes);
  Object closeRes = p_parenClose(thd, tokens);
  if (closeRes.a == nullObj.a) {
    p_fail(thd, tokens, "closing parenthesis expected");
  }
  tokens = listGetRest(closeRes);
  return listNew(expr, tokens);
}

Object p_queue(Thread* thd, Object tokens) {
  Object res = p_spotSpecial(tokens, "~");
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  res = p_list(thd, tokens);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "queue elements expected after '~'");
  }
  Object elems = listGetFirst(res);
  tokens = listGetRest(res);
  Object q = queueNew();
  listEach(elems, queueEnq, q);
  return listNew(q, tokens);
}

Object p_quote(Thread* thd, Object tokens) {
  Object res = p_singleQuote(thd, tokens);
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  res = p_any(thd, tokens);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "expression expected after open quote");
  }
  Object expr = listGetFirst(res);
  tokens = listGetRest(res);
  res = p_singleQuote(thd, tokens);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "closing quote expected after expression");
  }
  tokens = listGetRest(res);
  Object quote = quoteNew(expr);
  return listNew(quote, tokens);
}

Object p_set(Thread* thd, Object tokens) {
  Object res = p_spotSpecial(tokens, "$");
  if (res.a == nullObj.a) {
    return nullObj;
  }
  tokens = listGetRest(res);
  res = p_array(thd, tokens);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "set elements expected after '$'");
  }
  Object elems = listGetFirst(res);
  tokens = listGetRest(res);
  Object set = setNew();
  arrayEach(elems, setAddElem, set);
  return listNew(set, tokens);
}

/* any expression */
Object p_expr(Thread* thd, Object tokens) {
  Parser parsers[] = {p_parenExpr, p_apply, p_do, p_function, p_if, p_let, p_letRec, p_quote, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}

/* list of expressions */
Object p_listOfAny(Thread* thd, Object tokens) {
  Object res = p_some(thd, tokens, p_any, 1);
  if (res.a == nullObj.a) {
    p_fail(thd, tokens, "expression expected");
  }
  return res;
}

/* any object or expression */
Object p_any(Thread* thd, Object tokens) {
  Parser parsers[] = {p_expr, p_object, NULL};
  Object res = p_oneOf(thd, tokens, parsers);
  return res;
}
