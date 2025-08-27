%{
    #include <stdio.h>
    #include "./codeGen/codeGen.h"
    #include "./y.tab.h"
%}

%%

[0-9]+ {
    int val = atoi(yytext);
    yylval.node = createLeafNode(val);
    return NUMBER;
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
\n { 
    return NEWLINE; 
}

[ \t]+ {
}
. {
    return -1;
}

%%

int yywrap() {
    return 1;
}
