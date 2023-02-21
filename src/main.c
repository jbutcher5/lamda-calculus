#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

int main() {
  const char *text = "(\\x z -> z x x) (\\x y -> y x)";
  LexerResult lexed = lexer(text);
  ParserResult parsed = parser(lexed.buffer, lexed.size, text, NULL);

  print_ast(parsed, text);

  return 0;
}

