%{
  #include <stdio.h>
  int charCount=0, wordCount=0, lineCount=0;
%}

%%

[\n] {
  lineCount += 1;
  charCount += 1;
}

[^\n ]+ {
  wordCount += 1;
  charCount += (yyleng);
}

. {
  charCount += 1;
}

%%

int yywrap() {
  return 1;
}

int main(int argc, char** argv) {
  if(argc > 1) {
    FILE* fp =  fopen(argv[1], "r");
    yyin = fp;
  }
  else {
    yyin = stdin;
  }

  yylex();

  printf("Characters: %d\n", charCount);
  printf("Words: %d\n", wordCount);
  printf("Lines: %d\n", lineCount);
  return 0;
}