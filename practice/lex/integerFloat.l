%{
  #include <stdio.h>
  int floats=0, ints=0;
%}

number_seq [0-9]+

%%

[-]?{number_seq}\.{number_seq} {
  floats += 1;
}

[-]?{number_seq} {
  ints += 1;
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

  printf("Floats: %d\n", floats);
  printf("Integers: %d\n", ints);
  return 0;
}