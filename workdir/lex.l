%{
    #include <stdio.h>
    #include "./node/node.h"
    #include "./y.tab.h"
    #include "./define/constants.h"
%}

%%

[0-9]+ {
    int val = atoi(yytext);
    yylval.astNode = createIntNode(val);
    return NUMBER;
}

"=" { return ASSG; }

"(" { return LPAR; }
")" { return RPAR; }
"[" { return LBRACK; }
"]" { return RBRACK; }
"{" { return LCURL; }
"}" { return RCURL; }

"+" { return PLUS; }
"-" { return MIN; }
"*" { return MULT; }
"/" { return DIV; }
"%" { return MOD; }

"==" { return EQ; }
"!=" { return NEQ; }
">=" { return GTE; }
">" { return GT; }
"<=" { return LTE; }
"<" { return LT; }

"begin" { return BEGIN_CODE; }
"end" { return END_CODE; }

"decl" { return BEGIN_DECL; }
"enddecl" { return END_DECL; }

"type" { return BEGIN_TYPE; }
"endtype" { return END_TYPE; }

"read" { return READ; }
"write" { return WRITE; }

"if" { return IF; }
"then" { return THEN; }
"else" { return ELSE; }
"endif" { return ENDIF; }

"while" { return WHILE; }
"do" { return DO; }
"endwhile" { return ENDWHILE; }

"AND" { return AND; }
"OR"  { return OR; }

"repeat" { return REPEAT; }
"until" { return UNTIL; }

"&" { return AMPSAND; }
"." { return DOT; }

"break" { return BREAK; }
"continue" { return CONTINUE; }

"int" { return INT; }
"str" { return STR; }

"alloc" { return ALLOC; }
"free" { return FREE; }
"init" { return INITLZE; }

"main" { return MAIN; }

"return" { return RETURN; }

\"[^\"]+\" {
    yylval.astNode = createStrLtrlNode(strdup(yytext));
    return STR_LTRL;
}

[a-zA-Z][a-zA-Z0-9]* {
    yylval.astNode = createIdNode(strdup(yytext));
    return ID;
}

"," { return COMMA; }

";" { return EOL; }

\/\/.*\n {}
[ \t\n]+ {}
. { return -1; }

%%

int yywrap() {
    return 1;
}
