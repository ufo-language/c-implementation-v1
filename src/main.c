#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "mem.h"

void getLine(char* buffer, int len) {
  int i = 0;
  char c;
  while (i < len) {
    c = getchar();
    if (c == '\n') {
      buffer[i] = '\0';
      return;
    }
    buffer[i++] = c;
  }
  while ((c = getchar()) != '\n');
  printf("line too long, extra ignored\n");
}


#include "lexer.h"
#include "syntax.h"

/* test the lexer */
void testLexer() {
  memStart();
  globalsSetup();
  printf("> ");
  char line[128];
  getLine(line, 128);
  printf("line = '%s'\n", line);
  LexerState lexerState;
  lexInit(&lexerState, syntax, line);
  Token token;
  while (true) {
    bool res = lexToken(&lexerState, &token);
    if (lexerState.error) {
      printf("lexer error\n");
      break;
    }
    printf("\n");
    printf("type = %s\n", T_NAMES[token.type]);
    printf("lexeme = '%s'\n", token.lexeme);
    if (!res) {
      break;
    }
  }
}

#if 1

#include "parser.h"
#include "delegate.h"
#include "syntax.h"

void showStats();

int main(int argc, char** argv) {
  if (argc > 1) {
    if(!strcmp(argv[1], "lex")) {
      memStart();
      globalsSetup();
      lex(0);
      return 0;
    }
  }
  

  printf("main has been disabled\n");
#if 0
  memStart();  // calls vmemStart()
  globalsSetup();

  showStats();
  printf("Enter an integer, real, or quoted string\n");
  printf("> ");
  char line[128];
  getLine(line, 128);
  printf("main got line = '%s'\n", line);
  Object* obj = parse(line, syntax);
  show(obj);
  if (obj != NULL) {
    printf(" : %s", O_NAMES[obj->type]);
  }
  printf("\n");
  memStop();
#endif
  return 0;
}

void showStats() {
  printf("System stats:\n");
  printf("Size of char:  %ld bits\n", sizeof(char) * 8);
  printf("Size of int:   %ld bits\n", sizeof(int) * 8);
  printf("Size of long:  %ld bits\n", sizeof(long) * 8);
  printf("Size of void*: %ld bits\n", sizeof(void*) * 8);
  printf("Size of uint:  %ld bits\n", sizeof(uint) * 8);
  printf("----\n");
}

#endif
