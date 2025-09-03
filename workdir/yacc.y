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
    void initiateCodeGen(Tnode* node);
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
%token REPEAT UNTIL
%token BREAK CONTINUE
%token LPAR RPAR
%token EOL

%type <node> start 
%type <node> stmtList stmt 
%type <node> inputStmt outputStmt assignStmt 
%type <node> ifStmt whileStmt doWhileStmt repeatUntilStmt
%type <node> expr

%nonassoc EQ NEQ GT GTE LT LTE
%left PLUS MIN
%left MULT DIV
%nonassoc ASSG

%%

start :

    BEGIN_CODE stmtList END_CODE EOL {
        $$ = $2;
        initiateCodeGen($$);
    }|

    BEGIN_CODE END_CODE EOL {
        $$ = NULL;
        initiateCodeGen($$);
    };

stmtList :

    stmtList stmt {
        $$ = createTnode(NODE_CONNECTOR, NO_TYPE, -1, NULL, $1, $2);
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
    }|

    doWhileStmt {
        $$ = $1;
    }|

    repeatUntilStmt {
        $$ = $1;
    }|

    BREAK EOL{
        $$ = createTnode(NODE_BREAK, NO_TYPE, -1, NULL, NULL, NULL);
    }|

    CONTINUE EOL{
        $$ = createTnode(NODE_CONTINUE, NO_TYPE, -1, NULL, NULL, NULL);
    };

inputStmt :

    READ LPAR ID RPAR EOL {
        $$ = createTnode(NODE_READ, NO_TYPE, -1, NULL, $3, NULL);
    };

outputStmt :

    WRITE LPAR expr RPAR EOL {
        $$ = createTnode(NODE_WRITE, NO_TYPE, -1, NULL, $3, NULL);
    };

assignStmt :

    ID ASSG expr EOL {
        $$ = createTnode(NODE_ASSIGN, NO_TYPE, -1, NULL, $1, $3); 
    };

ifStmt :

    IF LPAR expr RPAR THEN stmtList ELSE stmtList ENDIF EOL {
        Tnode* conn = createTnode(NODE_CONNECTOR, NO_TYPE, -1, NULL, $6, $8);
        $$ = createTnode(NODE_IF_ELSE, NO_TYPE, -1, NULL, $3, conn);
    }|

    IF LPAR expr RPAR THEN stmtList ENDIF EOL {
        $$ = createTnode(NODE_IF, NO_TYPE, -1, NULL, $3, $6);
    };

whileStmt :

    WHILE LPAR expr RPAR DO stmtList ENDWHILE EOL {
        $$ = createTnode(NODE_WHILE, NO_TYPE, -1, NULL, $3, $6);
    };

doWhileStmt :

    DO stmtList WHILE LPAR expr RPAR EOL {
        $$ = createTnode(NODE_DOWHILE, NO_TYPE, -1, NULL, $5, $2);
    };

repeatUntilStmt : 

    REPEAT stmtList UNTIL LPAR expr RPAR EOL {
        $$ = createTnode(NODE_REPEATUNTIL, NO_TYPE, -1, NULL, $5, $2);
    };

expr :

    expr PLUS expr {
        $$ = createTnode(NODE_ADD, INTEGER_TYPE, -1, NULL, $1, $3);
    }|

    expr MIN expr {
        $$ = createTnode(NODE_SUB, INTEGER_TYPE, -1, NULL, $1, $3);
    }|

    expr MULT expr {
        $$ = createTnode(NODE_MULT, INTEGER_TYPE, -1, NULL, $1, $3);
    }|

    expr DIV expr {
        $$ = createTnode(NODE_DIV, INTEGER_TYPE, -1, NULL, $1, $3);
    }|

    expr EQ expr {
        $$ = createTnode(NODE_EQ, BOOLEAN_TYPE, -1, NULL, $1, $3);
    }|

    expr NEQ expr {
        $$ = createTnode(NODE_NEQ, BOOLEAN_TYPE, -1, NULL, $1, $3);
    }|

    expr GTE expr {
        $$ = createTnode(NODE_GTE, BOOLEAN_TYPE, -1, NULL, $1, $3);
    }|

    expr GT expr {
        $$ = createTnode(NODE_GT, BOOLEAN_TYPE, -1, NULL, $1, $3);
    }|

    expr LTE expr {
        $$ = createTnode(NODE_LTE, BOOLEAN_TYPE, -1, NULL, $1, $3);
    }|

    expr LT expr {
        $$ = createTnode(NODE_LT, BOOLEAN_TYPE, -1, NULL, $1, $3);
    }|

    LPAR expr RPAR {
        $$ = $2;
    }|

    ID {
        $$ = $1;
    }|

    NUMBER {
        $$ = $1;
    };

%%

int yyerror(const char* s) {
    printf("Error: %s\n", s);
    return 0;
}

void initiateCodeGen(Tnode* node) {
    FILE* targetFile = fopen("targetFile.xsm", "w");
    setHeader(targetFile);
    resetRegisters();
    newLabel = 0;
    updateStackPointer(4121, targetFile);
    codeGen(node, targetFile);
    exitProgram(targetFile);
    exit(1);
}

int main(int argc, char *argv[]) {
    if(argc > 1)
        yyin = fopen(argv[1], "r");
    else 
        yyin = stdin;
    
    yyparse();
    return 0;
}
