%{
  #include <stdio.h>
  #include <stdlib.h>
%}

number_sequence [0-9]*

%%

{number_sequence}[02468] {
  int evenNum = atoi(yytext);
  return evenNum;
}

{number_sequence}[13579] {
  int oddNum = atoi(yytext);
  return oddNum;
}

.|\n;

%%

int yywrap() {
  return 1;
}

int main() {
  printf("Enter one odd and one even number in any order: \n");
  int num1 = yylex();
  int num2 = yylex();

  if((num1-num2) % 2)
    printf("Good job\n");
  else
    printf("Bad boy\n");

  return 1;
}