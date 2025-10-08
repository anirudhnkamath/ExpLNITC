%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "./define/constants.h"
    #include "./codeGen/codeGen.h"
    #include "./registers/registers.h"
    #include "./gsTable/gsTable.h"
    #include "./node/node.h"
    #include "./gsTable/gsTable.h"
    #include "./xsmGen/xsmGen.h"

    FILE* yyin;
    int yylex(void);
    int yyerror(const char *s);
    void initiateCodeGen(Tnode* node);
%}

%union {
    struct Tnode* astNode;
    struct GsTableEntry* gsTableEntry;
    struct ParamListEntry* paramListEntry;
    int declDataType;
}

%token <astNode> ID NUMBER STR_LTRL
%token BEGIN_DECL END_DECL BEGIN_CODE END_CODE
%token ASSG
%token AMPSAND
%token PLUS MIN MULT DIV EQ NEQ GTE GT LTE LT MOD
%token IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL BREAK CONTINUE MAIN READ WRITE
%token LPAR RPAR LBRACK RBRACK LCURL RCURL
%token INT STR
%token EOL COMMA

%type <astNode> stmtList stmt
%type <astNode> inputStmt outputStmt assignStmt 
%type <astNode> ifStmt whileStmt doWhileStmt repeatUntilStmt
%type <astNode> expr
%type <astNode> arrIndex
%type <astNode> program fDefBlock mainBlock body

%type <gsTableEntry> gDeclBlock gDeclList gDecl gidList gid

%type <declDataType> type

%type <paramListEntry> paramList param

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

gDeclBlock  :   BEGIN_DECL gDeclList END_DECL       { gsTableHead = $2; printGsTable(); }
            |   BEGIN_DECL END_DECL                 { gsTableHead = NULL; }
            ;

gDeclList   :   gDeclList gDecl     { $$ = concatGsTable($1, $2); }
            |   gDecl               { $$ = $1; }
            ;

gDecl       :   type gidList EOL    { $$ = setGsTableType($2, $1); }
            ;

gidList     :   gidList COMMA gid   { $$ = concatGsTable($1, $3); }
            |   gid                 { $$ = $1; }
            ;

gid         :   ID                          { $$ = createIdEntryInGsTable($1->varName); }
            |   ID LBRACK NUMBER RBRACK     { $$ = createArrEntryInGsTable($1->varName, $3->val); }
            |   ID LPAR paramList RPAR      { $$ = createFnEntryInGsTable($1->varName, $3); }
            ;

type        :   INT     { $$ = INTEGER_TYPE; }
            |   STR     { $$ = STRING_TYPE; }
            ;



/*  FUNCTION DEFINITIONS  */

fDefBlock   :   fDefBlock fDef      { }
            |   fDef                { }
            ;

fDef        :   type ID LPAR paramList RPAR LCURL lDeclBlock body RCURL     { }
            ;

paramList   :   paramList COMMA param   { $$ = concatParamList($1, $3); }
            |   param                   { $$ = $1; }
            |                           { $$ = NULL; }
            ;

param       :   type ID     { $$ = createParamListEntry($2->varName, $1); }
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
    updateStackPointer(STACK_END, targetFile);
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
