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
%token BEGIN_DECL END_DECL
%token BEGIN_CODE END_CODE
%token READ WRITE
%token ASSG
%token AMPSAND
%token PLUS MIN MULT DIV
%token EQ NEQ GTE GT LTE LT MOD
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE
%token REPEAT UNTIL
%token BREAK CONTINUE
%token LPAR RPAR LBRACK RBRACK LCURL RCURL
%token INT STR
%token EOL COMMA
%token MAIN

%type <node> stmtList stmt
%type <node> inputStmt outputStmt assignStmt 
%type <node> ifStmt whileStmt doWhileStmt repeatUntilStmt
%type <node> expr
%type <node> arrIndex

%type <node> program gDeclBlock fDefBlock mainBlock gDeclList body

%nonassoc EQ NEQ GT GTE LT LTE
%left PLUS MIN
%left MULT DIV MOD
%nonassoc ASSG

%%

program     :   gDeclBlock fDefBlock mainBlock  { }
            |   gDeclBlock mainBlock            { }
            |   mainBlock                       { }
            ;



/*  GLOBAL DECLARATIONS  */

gDeclBlock  :   BEGIN_DECL gDeclList END_DECL       { }
            |   BEGIN_DECL END_DECL                 { }
            ;

gDeclList   :   gDeclList gDecl     { }
            |   gDecl               { }
            ;

gDecl       :   type gidList EOL    { }
            ;

gidList     :   gidList COMMA gid   { }
            |   gid                 { }
            ;

gid         :   ID                          { }
            |   ID LBRACK NUMBER RBRACK     { }
            |   ID LPAR paramList RPAR      { }
            ;

type        :   INT     { }
            |   STR     { }
            ;



/*  FUNCTION DEFINITIONS  */

fDefBlock   :   fDefBlock fDef      { }
            |   fDef                { }
            ;

fDef        :   type ID LPAR paramList RPAR LCURL lDeclBlock body RCURL     { }
            ;

paramList   :   paramList COMMA param   { }
            |   param                   { }
            |                           { }
            ;

param       :   type ID     { }
            ;



/*  LOCAL DECLARATIONS  */

lDeclBlock  :   BEGIN_DECL lDeclList END_DECL       { }
            |   BEGIN_DECL END_DECL                 { }
            ;

lDeclList   :   lDeclList lDecl     { }
            |   lDecl               { }
            ;

lDecl       :   type idList EOL     { }
            ;

idList      :   idList COMMA ID     { }
            |   ID                  { } 
            ;



/*  MAIN BLOCK  */

mainBlock   :   type MAIN LPAR RPAR LCURL lDeclBlock body RCURL     { }
            ;



/*  BODY  */

body        :   BEGIN_CODE stmtList END_CODE        { }
            |   BEGIN_CODE END_CODE                 { }
            ;

stmtList    :   stmtList stmt       { }
            |   stmt                { }
            ;

stmt        :   inputStmt           { }
            |   outputStmt          { }
            |   assignStmt          { }
            |   ifStmt              { }
            |   whileStmt           { }
            |   doWhileStmt         { }
            |   repeatUntilStmt     { }
            |   BREAK EOL           { }
            |   CONTINUE EOL        { }
            ;

inputStmt   :   READ LPAR ID RPAR EOL           { }
            |   READ LPAR arrIndex RPAR EOL     { }
            ;

outputStmt  :   WRITE LPAR expr RPAR EOL        { }
            ;

assignStmt  :   ID ASSG expr EOL            { }
            |   arrIndex ASSG expr EOL      { }
            |   ID ASSG AMPSAND ID EOL      { }
            |   MULT ID ASSG expr EOL       { }
            ;

ifStmt      :   IF LPAR expr RPAR THEN stmtList ELSE stmtList ENDIF EOL         { }
            |   IF LPAR expr RPAR THEN stmtList ENDIF EOL                       { }
            ;

whileStmt       :   WHILE LPAR expr RPAR DO stmtList ENDWHILE EOL   { }
                ;
doWhileStmt     :   DO stmtList WHILE LPAR expr RPAR EOL            { }
                ;
repeatUntilStmt :   REPEAT stmtList UNTIL LPAR expr RPAR EOL        { }
                ;

arrIndex    :   ID LBRACK expr RBRACK                       { }
            ;

expr        :   expr PLUS expr          { }
            |   expr MIN expr           { }
            |   expr MULT expr          { }
            |   expr DIV expr           { }
            |   expr MOD expr           { }
            |   expr EQ expr            { }
            |   expr NEQ expr           { }
            |   expr GTE expr           { }
            |   expr GT expr            { }
            |   expr LTE expr           { }
            |   expr LT expr            { }
            |   LPAR expr RPAR          { }
            |   ID                      { }
            |   arrIndex                { }
            |   NUMBER                  { }
            |   STR_LTRL                { }
            |   MULT ID                 { }
            |   ID LPAR RPAR            { }
            |   ID LPAR argList RPAR    { }
            ;

argList     :   argList COMMA expr      { }
            |   expr                    { }
            ;

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
