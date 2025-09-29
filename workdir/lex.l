%{
    #include <stdio.h>
    #include "./node/node.h"
    #include "./y.tab.h"
    #include "./define/constants.h"
%}

%%

[0-9]+ {
    int val = atoi(yytext);
    yylval.node = createIntNode(val);
    return NUMBER;
}

"=" { return ASSG; }

"(" { return LPAR; }
")" { return RPAR; }

"[" { return LBRACK; }
"]" { return RBRACK; }

"+" { return PLUS; }
"-" { return MIN; }
"*" { return MULT; }
"/" { return DIV; }

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

"read" { return READ; }
"write" { return WRITE; }

"if" { return IF; }
"then" { return THEN; }
"else" { return ELSE; }
"endif" { return ENDIF; }

"while" { return WHILE; }
"do" { return DO; }
"endwhile" { return ENDWHILE; }

"repeat" { return REPEAT; }
"until" { return UNTIL; }

"break" { return BREAK; }
"continue" { return CONTINUE; }

"int" { return INT; }
"str" { return STR; }

\"[^\"]+\" {
    yylval.node = createStrLtrlNode(strdup(yytext));
    return STR_LTRL;
}

[a-z][a-z0-9]* {
    yylval.node = createIdNode(strdup(yytext));
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
