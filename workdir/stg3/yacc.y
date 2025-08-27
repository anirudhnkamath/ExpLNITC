%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "./define/constants.h"
    #include "./codeGen/codeGen.h"
    #include "./registers/registers.h"
    #include "./node/node.h"

    FILE* yyin;

    int yylex(void);
    int yyerror(const char *s);
%}

%union {
    struct Tnode* node;
}

%token <node> ID NUMBER
%token BEGIN_CODE END_CODE 
%token READ WRITE
%token ASSG
%token PLUS MIN MULT DIV
%token EQ NEQ GTE GT LTE LT
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token LPAR RPAR
%token EOL

%type <node> start 
%type <node> stmtList stmt 
%type <node> inputStmt outputStmt assignStmt 
%type <node> ifStmt whileStmt
%type <node> expr

%left PLUS MIN
%left MULT DIV
%left EQ NEQ GT GTE LT LTE
%nonassoc ASSG

%%

start :

    BEGIN_CODE stmtList END_CODE EOL {
        $$ = $2;
        inorder($$);
    }|

    BEGIN_CODE END_CODE EOL {
        $$ = NULL;
        inorder($$);
    };

stmtList :

    stmtList stmt {
        $$ = createTnode(NODE_CONNECTOR, -1, NULL, $1, $2);
    }|

    stmt {
        $$ = $1;
    };

stmt :

    inputStmt {
        $$ = $1;
    }|

    outputStmt {
        $$ = $1;
    }|

    assignStmt {
        $$ = $1;
    }|

    ifStmt {
        $$ = $1;
    }|

    whileStmt {
        $$ = $1;
    };

inputStmt :

    READ LPAR ID RPAR EOL {
        $$ = createTnode(NODE_READ, -1, NULL, $3, NULL);
    };

outputStmt :

    WRITE LPAR expr RPAR EOL {
        $$ = createTnode(NODE_WRITE, -1, NULL, $3, NULL);
    };

assignStmt :

    ID ASSG expr EOL {
        $$ = createTnode(NODE_ASSIGN, -1, NULL, $1, $3); 
    };

ifStmt :

    IF LPAR expr RPAR THEN stmtList ELSE stmtList ENDIF EOL {
        Tnode* conn = createTnode(NODE_CONNECTOR, -1, NULL, $6, $8);
        $$ = createTnode(NODE_IF_ELSE, -1, NULL, $3, conn);
    }|

    IF LPAR expr RPAR THEN stmtList ENDIF EOL {
        $$ = createTnode(NODE_IF, -1, NULL, $3, $6);
    };

whileStmt :

    WHILE LPAR expr RPAR DO stmtList ENDWHILE EOL {
        $$ = createTnode(NODE_WHILE, -1, NULL, $3, $6);
    };

expr :

    expr PLUS expr {
        $$ = createTnode(NODE_ADD, -1, NULL, $1, $3);
    }|

    expr MIN expr {
        $$ = createTnode(NODE_SUB, -1, NULL, $1, $3);
    }|

    expr MULT expr {
        $$ = createTnode(NODE_MULT, -1, NULL, $1, $3);
    }|

    expr DIV expr {
        $$ = createTnode(NODE_DIV, -1, NULL, $1, $3);
    }|

    expr EQ expr {
        $$ = createTnode(NODE_EQ, -1, NULL, $1, $3);
    }|

    expr NEQ expr {
        $$ = createTnode(NODE_NEQ, -1, NULL, $1, $3);
    }|

    expr GTE expr {
        $$ = createTnode(NODE_GTE, -1, NULL, $1, $3);
    }|

    expr GT expr {
        $$ = createTnode(NODE_GT, -1, NULL, $1, $3);
    }|

    expr LTE expr {
        $$ = createTnode(NODE_LTE, -1, NULL, $1, $3);
    }|

    expr LT expr {
        $$ = createTnode(NODE_LT, -1, NULL, $1, $3);
    }|

    ID {
        $$ = $1;
    }|

    NUMBER {
        $$ = $1;
    };

%%

int yyerror(const char* s) {
    printf("Error occured\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc > 1)
        yyin = fopen(argv[1], "r");
    else 
        yyin = stdin;
    
    yyparse();
    return 0;
}
