%option noyywrap
%{
    #include <stdlib.h>
    #include <stdio.h>
%}

number [0-9]+
other .|\n

%%

{number} {
    int num = atoi(yytext);
    if(num%2 == 0)
        printf("Even and size %d\n", yyleng);
    else
        printf("Odd and size %d\n", yyleng);
}

{other} ;

%%

int main() {
    yylex();
    return 1;
}

