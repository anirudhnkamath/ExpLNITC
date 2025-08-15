%{
	#include<stdio.h>
  #include <stdlib.h>
	char *filename = "1.txt";
%}

%%

[0-9]+ 	printf("%d\n", atoi(yytext));
.|\n ;

%%

int yywrap()
{
  if(yyin == stdin) {
    yyin = fopen(filename, "r");
    return 0;
  }
  return 1;
}

int main()
{
  	yylex();
  	return 1;
}