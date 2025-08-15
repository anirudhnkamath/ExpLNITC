%option noyywrap
%{
	#include <stdlib.h>
	#include <stdio.h>
%}
	
number [0-9]+
 
%%

{number} {printf("Hello\n");}

%%

int main()
{
	int num = yylex();
	printf("Found: %d",num);
	return 1;
}