%{
  #include <stdio.h>
%}

%%

\/\/[^\n]* {
	printf("Single line:\n");
  printf("%s\n", yytext);
  printf("\n\n");
}

\/\*([^*]|\*+[^/])*\*\/ {
	printf("Multi line: \n");
  printf("%s\n", yytext);
  printf("\n\n");
}

.|\n {
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
  return 0;
}
