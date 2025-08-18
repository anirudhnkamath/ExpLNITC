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
%token BEGIN_CODE END_CODE READ WRITE
%token ASSIGN PLUS MIN MULT DIV
%token LBRACE RBRACE EOL

%type <node> start stmtList stmt inputStmt outputStmt assignStmt expr

%left PLUS MIN
%left MULT DIV
%nonassoc ASSIGN

%%

start :

    BEGIN_CODE stmtList END_CODE EOL {
        $$ = $2;
        FILE* targetFile = fopen("target.xsm", "w");
        setHeader(targetFile);
        resetRegisters();
        updateStackPointer(4121, targetFile);
        codeGen($$, targetFile);
        exitProgram(targetFile);

        exit(1);
    }|

    BEGIN_CODE END_CODE EOL {
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
    };

inputStmt :

    READ LBRACE ID RBRACE EOL {
        $$ = createTnode(NODE_READ, -1, NULL, $3, NULL);
    };

outputStmt :

    WRITE LBRACE expr RBRACE EOL {
        $$ = createTnode(NODE_WRITE, -1, NULL, $3, NULL);
    };

assignStmt :

    ID ASSIGN expr EOL {
        $$ = createTnode(NODE_ASSIGN, -1, NULL, $1, $3); 
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
