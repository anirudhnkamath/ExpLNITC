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
    struct LsTableEntry* lsTableEntry;
    int declDataType;
}

%token <astNode> ID NUMBER STR_LTRL
%token BEGIN_DECL END_DECL BEGIN_CODE END_CODE
%token ASSG
%token PLUS MIN MULT DIV EQ NEQ GTE GT LTE LT MOD
%token IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL BREAK CONTINUE MAIN READ WRITE
%token LPAR RPAR LBRACK RBRACK LCURL RCURL
%token INT STR
%token EOL COMMA

%type <astNode> stmtList stmt
%type <astNode> inputStmt outputStmt assignStmt 
%type <astNode> ifStmt whileStmt doWhileStmt rptUntStmt
%type <astNode> expr
%type <astNode> arrIndex
%type <astNode> body fDef argList mainBlock

%type <gsTableEntry> gDeclBlock gDeclList gDecl gidList gid
%type <lsTableEntry> idList lDecl lDeclList

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

gDeclBlock  :   BEGIN_DECL gDeclList END_DECL       { gsTableHead = $2; }
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

fDef        :   type ID LPAR paramList RPAR LCURL lDeclBlock    { lsTableHead = addParamsToLsTable(lsTableHead, $4); } 
                        /* mid-rule action */      body RCURL   {   
                                                                    functionValidate($1, $2->varName, $4);
                                                                    $$ = $9;
                                                                    // codegen here i guess
                                                                    freeLsTable();
                                                                    freeTree($$);
                                                                } 
            ;

paramList   :   paramList COMMA param   { $$ = concatParamList($1, $3); }
            |   param                   { $$ = $1; }
            |                           { $$ = NULL; }
            ;

param       :   type ID     { $$ = createParamListEntry($2->varName, $1); }
            ;



/*  LOCAL DECLARATIONS  */

lDeclBlock  :   BEGIN_DECL lDeclList END_DECL       { lsTableHead = $2; }
            |   BEGIN_DECL END_DECL                 { lsTableHead = NULL; }
            ;

lDeclList   :   lDeclList lDecl     { $$ = concatLsTable($1, $2); }
            |   lDecl               { $$ = $1; }
            ;

lDecl       :   type idList EOL     { $$ = setLsTableType($2, $1); }
            ;

idList      :   idList COMMA ID     { $$ = concatLsTable($1, createIdEntryInLsTable($3->varName)); }
            |   ID                  { $$ = createIdEntryInLsTable($1->varName); } 
            ;



/*  MAIN BLOCK  */

mainBlock   :   type MAIN LPAR RPAR LCURL lDeclBlock body RCURL     { $$ = $7; }
            ;



/*  BODY  */

body        :   BEGIN_CODE stmtList END_CODE    { $$ = $2; }
            |   BEGIN_CODE END_CODE             { $$ = NULL; }
            ;

stmtList    :   stmtList stmt                   { $$ = createConnectorNode($1, $2); }
            |   stmt                            { $$ = $1; }
            ;

stmt        :   inputStmt                       { $$ = $1; }
            |   outputStmt                      { $$ = $1; }
            |   assignStmt                      { $$ = $1; }
            |   ifStmt                          { $$ = $1; }
            |   whileStmt                       { $$ = $1; }
            |   doWhileStmt                     { $$ = $1; }
            |   rptUntStmt                      { $$ = $1; }
            |   BREAK EOL                       { $$ = createBreakNode(); }
            |   CONTINUE EOL                    { $$ = createContinueNode(); }
            ;

inputStmt   :   READ LPAR ID RPAR EOL           { setIdNodeType($3); validateIdForExpr($3); $$ = createReadNode($3); }
            |   READ LPAR arrIndex RPAR EOL     { $$ = createReadNode($3); }
            ;

outputStmt  :   WRITE LPAR expr RPAR EOL        { $$ = createWriteNode($3); }
            ;

assignStmt  :   ID ASSG expr EOL                { setIdNodeType($1); validateIdForExpr($1); $$ = createAssignNode($1, $3); }
            |   arrIndex ASSG expr EOL          { $$ = createAssignNode($1, $3); }
            ;

ifStmt      :   IF LPAR expr RPAR THEN stmtList ELSE stmtList ENDIF EOL         { $$ = createIfElseNode($3, $6, $8); }
            |   IF LPAR expr RPAR THEN stmtList ENDIF EOL                       { $$ = createIfNode($3, $6); }
            ;

whileStmt   :   WHILE LPAR expr RPAR DO stmtList ENDWHILE EOL                   { $$ = createWhileNode($3, $6); }
            ;
doWhileStmt :   DO stmtList WHILE LPAR expr RPAR EOL                            { $$ = createDoWhileNode($2, $5); }
            ;
rptUntStmt  :   REPEAT stmtList UNTIL LPAR expr RPAR EOL                        { $$ = createRepeatUntilNode($2, $5); }
            ;

arrIndex    :   ID LBRACK expr RBRACK           { setIdNodeType($1); $$ = createArrIndexNode($1, $3); }
            ;

expr        :   expr PLUS expr          { $$ = createArithOpNode(NODE_ADD, $1, $3); }
            |   expr MIN expr           { $$ = createArithOpNode(NODE_SUB, $1, $3); }
            |   expr MULT expr          { $$ = createArithOpNode(NODE_MULT, $1, $3); }
            |   expr DIV expr           { $$ = createArithOpNode(NODE_DIV, $1, $3); }
            |   expr MOD expr           { $$ = createArithOpNode(NODE_MOD, $1, $3); }
            |   expr EQ expr            { $$ = createRelOpNode(NODE_EQ, $1, $3); }
            |   expr NEQ expr           { $$ = createRelOpNode(NODE_NEQ, $1, $3);}
            |   expr GTE expr           { $$ = createRelOpNode(NODE_GTE, $1, $3);}
            |   expr GT expr            { $$ = createRelOpNode(NODE_GT, $1, $3);}
            |   expr LTE expr           { $$ = createRelOpNode(NODE_LTE, $1, $3);}
            |   expr LT expr            { $$ = createRelOpNode(NODE_LT, $1, $3);}
            |   LPAR expr RPAR          { $$ = $2; }
            |   ID                      { setIdNodeType($1); validateIdForExpr($1); $$ = $1; }
            |   arrIndex                { $$ = $1; }
            |   NUMBER                  { $$ = $1; }
            |   STR_LTRL                { $$ = $1; }
            |   ID LPAR RPAR            { setIdNodeType($1); $$ = createFnCallNode($1, NULL); }
            |   ID LPAR argList RPAR    { setIdNodeType($1); $$ = createFnCallNode($1, $3); }
            ;

argList     :   argList COMMA expr      { $$ = addArgToArgList($1, $3); }
            |   expr                    { $$ = $1; }
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
