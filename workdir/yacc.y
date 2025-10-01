%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "./define/constants.h"
    #include "./codeGen/codeGen.h"
    #include "./registers/registers.h"
    #include "./node/node.h"
    #include "./gsTable/gsTable.h"
    #include "./xsmGen/xsmGen.h"

    FILE* yyin;

    int yylex(void);
    int yyerror(const char *s);
    void initiateCodeGen(Tnode* node);

    int curDeclarationType = INTEGER_TYPE;
%}

%union {
    struct Tnode* node;
}

%token <node> ID NUMBER STR_LTRL
%token BEGIN_CODE END_CODE BEGIN_DECL END_DECL
%token READ WRITE
%token ASSG
%token PLUS MIN MULT DIV
%token EQ NEQ GTE GT LTE LT
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL
%token BREAK CONTINUE
%token LPAR RPAR LBRACK RBRACK
%token INT STR
%token EOL COMMA

%type <node> start 
%type <node> declSection codeSection
%type <node> stmtList stmt declList decl
%type <node> inputStmt outputStmt assignStmt 
%type <node> ifStmt whileStmt doWhileStmt repeatUntilStmt
%type <node> expr
%type <node> arrIndex

%nonassoc EQ NEQ GT GTE LT LTE
%left PLUS MIN
%left MULT DIV
%nonassoc ASSG

%%

start :

    declSection codeSection {
        $$ = $2;
        initiateCodeGen($$);
        exit(0);
    };

declSection :

    BEGIN_DECL declList END_DECL EOL {
        $$ = $2;
        declarationOverFlag = 1;
    }|

    BEGIN_DECL END_DECL EOL {
        $$ = NULL;
        declarationOverFlag = 1;
    };

codeSection :

    BEGIN_CODE stmtList END_CODE EOL {
        $$ = $2;
    }|

    BEGIN_CODE END_CODE EOL {
        $$ = NULL;
    };

declList :

    declList decl {
    }|

    decl {
    };

decl :

    dataType varList EOL {
    };

dataType :

    INT {
        curDeclarationType = INTEGER_TYPE;
    }|

    STR {
        curDeclarationType = STRING_TYPE;
    };

varList :

    varList COMMA ID {
        gsTableHead = insertToGsTable(gsTableHead, ($3)->varName, curDeclarationType, 1, 0);
    }|

    varList COMMA ID LBRACK NUMBER RBRACK {
        gsTableHead = insertToGsTable(gsTableHead, ($3)->varName, curDeclarationType, ($5)->val, 1);
    }|

    ID LBRACK NUMBER RBRACK {
        gsTableHead = insertToGsTable(gsTableHead, ($1)->varName, curDeclarationType, ($3)->val, 1);
    }|

    ID {
        gsTableHead = insertToGsTable(gsTableHead, ($1)->varName, curDeclarationType, 1, 0);
    };

stmtList :

    stmtList stmt {
        $$ = createConnectorNode($1, $2);
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
        $$ = createBreakNode();
    }|

    CONTINUE EOL{
        $$ = createContinueNode();
    };

inputStmt :

    READ LPAR ID RPAR EOL {
        $$ = createReadNode($3);
    }|

    READ LPAR arrIndex RPAR EOL {
        $$ = createReadNode($3);
    };

outputStmt :

    WRITE LPAR expr RPAR EOL {
        $$ = createWriteNode($3);
    };

assignStmt :

    ID ASSG expr EOL {
        $$ = createAssignNode($1, $3);
    }|

    arrIndex ASSG expr EOL {
        $$ = createAssignNode($1, $3);
    };

ifStmt :

    IF LPAR expr RPAR THEN stmtList ELSE stmtList ENDIF EOL {
        $$ = createIfElseNode($3, $6, $8);
    }|

    IF LPAR expr RPAR THEN stmtList ENDIF EOL {
        $$ = createIfNode($3, $6);
    };

whileStmt :

    WHILE LPAR expr RPAR DO stmtList ENDWHILE EOL {
        $$ = createWhileNode($3, $6);
    };

doWhileStmt :

    DO stmtList WHILE LPAR expr RPAR EOL {
        $$ = createDoWhileNode($5, $2);
    };

repeatUntilStmt : 

    REPEAT stmtList UNTIL LPAR expr RPAR EOL {
        $$ = createRepeatUntilNode($5, $2);
    };

arrIndex :

    ID LBRACK expr RBRACK {
        $$ = createArrIndexNode($1, $3);
    };

expr :

    expr PLUS expr {
        $$ = createArithOpNode(NODE_ADD, $1, $3);
    }|

    expr MIN expr {
        $$ = createArithOpNode(NODE_SUB, $1, $3);
    }|

    expr MULT expr {
        $$ = createArithOpNode(NODE_MULT, $1, $3);
    }|

    expr DIV expr {
        $$ = createArithOpNode(NODE_DIV, $1, $3);
    }|

    expr EQ expr {
        $$ = createRelOpNode(NODE_EQ, $1, $3);
    }|

    expr NEQ expr {
        $$ = createRelOpNode(NODE_NEQ, $1, $3);
    }|

    expr GTE expr {
        $$ = createRelOpNode(NODE_GTE, $1, $3);
    }|

    expr GT expr {
        $$ = createRelOpNode(NODE_GT, $1, $3);
    }|

    expr LTE expr {
        $$ = createRelOpNode(NODE_LTE, $1, $3);
    }|

    expr LT expr {
        $$ = createRelOpNode(NODE_LT, $1, $3);
    }|

    LPAR expr RPAR {
        $$ = $2;
    }|

    ID {
        $$ = $1;
    }|

    arrIndex {
        $$ = $1;
    }|

    NUMBER {
        $$ = $1;
    }|

    STR_LTRL {
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
    updateStackPointer(STATIC_ALLOC_END, targetFile);
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
