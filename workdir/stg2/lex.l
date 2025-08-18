%{
    #include <stdio.h>
    #include "./node/node.h"
    #include "./y.tab.h"
    #include "./define/constants.h"
%}

%%

[0-9]+ {
    int val = atoi(yytext);
    yylval.node = createTnode(NODE_INT, val, NULL, NULL, NULL);
    return NUMBER;
}
"=" {
    return ASSIGN;
}
"(" {
    return LBRACE; 
}
")" {
    return RBRACE; 
}
"+" {
    return PLUS; 
}
"-" {
    return MIN; 
}
"*" {
    return MULT; 
}
"/" {
    return DIV; 
}

"begin" {
    return BEGIN_CODE;
}
"end" {
    return END_CODE;
}

"read" {
    return READ;
}

"write" {
    return WRITE;
}

[a-z] {
    yylval.node = createTnode(NODE_ID_INT, -1, strdup(yytext), NULL, NULL);
    return ID;
}

";" {
    return EOL; 
}

[ \t\n]+ {
}
. {
    return -1;
}

%%

int yywrap() {
    return 1;
}
