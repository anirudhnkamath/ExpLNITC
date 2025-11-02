%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "./define/constants.h"
    #include "./codeGen/codeGen.h"
    #include "./registers/registers.h"
    #include "./gsTable/gsTable.h"
    #include "./node/node.h"
    #include "./xsmGen/xsmGen.h"
    #include "./typeTable/typeTable.h"

    FILE* yyin;
    FILE* targetFile;
    int yylex(void);
    int yyerror(const char *s);
    void initiateCodeGen(Tnode* node);
%}

%union {
    struct Tnode* astNode;
    struct GsTableEntry* gsTableEntry;
    struct ParamListEntry* paramListEntry;
    struct LsTableEntry* lsTableEntry;
    struct TypeTable* declDataType;
    struct FieldList* fieldList;
    struct TypeTable* typeTable;
}

%token <astNode> ID NUMBER STR_LTRL
%token BEGIN_DECL END_DECL BEGIN_CODE END_CODE BEGIN_TYPE END_TYPE
%token ASSG
%token PLUS MIN MULT DIV EQ NEQ GTE GT LTE LT MOD AND OR AMPSAND
%token IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL BREAK CONTINUE MAIN READ WRITE RETURN ALLOC FREE INITLZE BRKP NULL_VAL
%token LPAR RPAR LBRACK RBRACK LCURL RCURL
%token INT STR
%token EOL COMMA DOT

%type <astNode> stmtList stmt
%type <astNode> inputStmt outputStmt assignStmt 
%type <astNode> ifStmt whileStmt doWhileStmt rptUntStmt
%type <astNode> expr
%type <astNode> body fDef argList mainBlock retStmt arrDimn arrIndex tupEntry

%type <gsTableEntry> gDeclBlock gDeclList gDecl gidList gid
%type <lsTableEntry> idList lDecl lDeclList

%type <declDataType> type

%type <paramListEntry> paramList param

%type <fieldList> field fieldList
%type <typeTable> tDeclBlock tDeclList


%left OR
%left AND
%nonassoc EQ NEQ GT GTE LT LTE
%left PLUS MIN
%left MULT DIV MOD
%left DOT
%nonassoc ASSG

%%

program     :   tDeclBlock gDeclBlock fDefBlock mainBlock  { }
            |   tDeclBlock gDeclBlock mainBlock            { }
            |   tDeclBlock mainBlock                       { }
            |   gDeclBlock fDefBlock mainBlock             { }
            |   gDeclBlock mainBlock                       { }
            |   mainBlock                                  { }
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
            |   ID LPAR paramList RPAR      { $$ = createFnEntryInGsTable($1->varName, $3); }
            |   ID arrDimn                  { $$ = createArrEntryInGsTable($1->varName, $2); }
            |   MULT ID                     { $$ = createPtrEntryInGsTable($2->varName); }
            ;

type        :   INT     { $$ = searchInTypeTable(typeTableHead, "int"); }
            |   STR     { $$ = searchInTypeTable(typeTableHead, "str"); }
            |   ID      { $$ = validateUserType(typeTableHead, $1->varName); }
            ;

arrDimn     :   arrDimn LBRACK NUMBER RBRACK    { $$ = insertToArrDimn($1, $3); }
            |   LBRACK NUMBER RBRACK            { $$ = $2; }
            ;      



/* TYPE DECLARATION */

tDeclBlock  :   BEGIN_TYPE tDeclList END_TYPE   { }
            |   BEGIN_TYPE END_TYPE             { }
            ;

tDeclList   :   tDeclList ID                            { typeTableHead = concatTypeTable(typeTableHead, createNewType($2->varName, NULL)); }
                LCURL fieldList RCURL                   {
                                                            $5 = setFieldIndex($5);
                                                            updateType($2->varName, $5);
                                                        }
                                                    
            |   ID                                      { typeTableHead = concatTypeTable(typeTableHead, createNewType($1->varName, NULL)); }
                LCURL fieldList RCURL                   {
                                                            $4 = setFieldIndex($4);
                                                            updateType($1->varName, $4);
                                                        }
            ;

fieldList   :   fieldList field                 { $$ = concatFieldList($1, $2); }
            |   field                           { $$ = $1; }

field       :   type ID EOL                      { $$ = createField($2->varName, $1); }
            ;


/*  FUNCTION DEFINITIONS  */

fDefBlock   :   fDefBlock fDef      { }
            |   fDef                { }
            ;

fDef        :   type ID LPAR paramList RPAR LCURL lDeclBlock    {
                                                                    validateFunction($1, $2, $4);
                                                                    
                                                                    setLDeclBinding(lsTableHead);
                                                                    functionEntryCodeGen($2, targetFile);

                                                                    lsTableHead = addParamsToLsTable(lsTableHead, $4);
                                                                } 
                /* mid-rule action */               body RCURL  {
                                                                    $$ = $9;
                                                                    codeGen($$, targetFile);
                                                                    functionExitCodeGen($2, targetFile);

                                                                    lsTableHead = NULL;
                                                                } 
            ;

paramList   :   paramList COMMA param   { $$ = concatParamList($1, $3); }
            |   param                   { $$ = $1; }
            |                           { $$ = NULL; }
            ;

param       :   type ID         {   $$ = createParamListEntry($2->varName, $1, _NA_); }
            |   type MULT ID    {   $$ = createParamListEntry($3->varName, $1, 1); }
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

idList      :   idList COMMA ID         { $$ = concatLsTable($1, createIdEntryInLsTable($3->varName)); }
            |   ID                      { $$ = createIdEntryInLsTable($1->varName); } 
            |   idList COMMA MULT ID    { $$ = concatLsTable($1, createPtrEntryInLsTable($4->varName)); }
            |   MULT ID                 { $$ = createPtrEntryInLsTable($2->varName); } 
            ;



/*  MAIN BLOCK  */

mainBlock   :   INT {curFnType = searchInTypeTable(typeTableHead, "int"); } MAIN
                                         LPAR RPAR LCURL lDeclBlock body RCURL     {
                                                                                        $$ = $8; 
                                                                                        setLDeclBinding(lsTableHead);

                                                                                        initialiseMainFn(targetFile);
                                                                                        codeGen($8, targetFile); 
                                                                                    }
            ;



/*  BODY  */

body        :   BEGIN_CODE stmtList retStmt END_CODE    { $$ = createConnectorNode($2, $3); }
            |   BEGIN_CODE retStmt END_CODE             { $$ = $2; }
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
            |   BREAK EOL                       { $$ = createLoopJumpNode(NODE_BREAK); }
            |   CONTINUE EOL                    { $$ = createLoopJumpNode(NODE_CONTINUE); }
            |   FREE LPAR expr RPAR EOL         { $$ = createFreeNode($3); }
            |   INITLZE LPAR RPAR EOL           { $$ = createInitlzeNode(); }
            |   BRKP EOL                        { $$ = createBrkpNode(); }
            ;

inputStmt   :   READ LPAR ID RPAR EOL           {    
                                                    $$ = createReadNode($3); 
                                                }
            |   READ LPAR ID arrIndex RPAR EOL  {   
                                                    $3->arrOffset = $4;
                                                    $$ = createReadNode($3); 
                                                } 
            |   READ LPAR tupEntry RPAR EOL     {
                                                    $$ = createReadNode($3);
                                                }
            ;

outputStmt  :   WRITE LPAR expr RPAR EOL        { $$ = createWriteNode($3); }
            ;

assignStmt  :   ID ASSG expr EOL                {   
                                                    setIdNodeType($1);
                                                    $$ = createAssignNode($1, $3); 
                                                }
            |   ID arrIndex ASSG expr EOL       {
                                                    setIdNodeType($1);
                                                    validateArrOffset($1, $2);
                                                    $1->arrOffset = $2;
                                                    $$ = createAssignNode($1, $4); 
                                                }
            |   MULT ID ASSG expr EOL           {
                                                    $$ = createAssignNode(createDerefNode($2), $4);
                                                }
            |   tupEntry ASSG expr EOL          {
                                                    $$ = createAssignNode($1, $3);
                                                }
            |   ID ASSG ALLOC LPAR RPAR EOL     {   
                                                    setIdNodeType($1);
                                                    $$ = createAssignNode($1, createAllocNode()); 
                                                }

            |   tupEntry ASSG ALLOC LPAR RPAR EOL       {  
                                                            $$ = createAssignNode($1, createAllocNode()); 
                                                        }
            
            |   ID arrIndex ASSG ALLOC LPAR RPAR EOL    {   
                                                            setIdNodeType($1);
                                                            validateArrOffset($1, $2);
                                                            $1->arrOffset = $2;
                                                            $$ = createAssignNode($1, createAllocNode()); 
                                                        }
            ;

ifStmt      :   IF LPAR expr RPAR THEN stmtList ELSE stmtList ENDIF EOL         { $$ = createIfElseNode($3, $6, $8); }
            |   IF LPAR expr RPAR THEN stmtList ENDIF EOL                       { $$ = createIfNode($3, $6); }
            ;

whileStmt   :   WHILE LPAR expr RPAR DO stmtList ENDWHILE EOL                   { $$ = createLoopNode(NODE_WHILE, $3, $6); }
            ;
doWhileStmt :   DO stmtList WHILE LPAR expr RPAR EOL                            { $$ = createLoopNode(NODE_DOWHILE, $2, $5); }
            ;
rptUntStmt  :   REPEAT stmtList UNTIL LPAR expr RPAR EOL                        { $$ = createLoopNode(NODE_REPEATUNTIL, $2, $5); }
            ;

retStmt     :   RETURN expr EOL       { $$ = createReturnNode($2); }
            ;

expr        :   expr PLUS expr          { $$ = createArithOpNode(NODE_ADD, $1, $3); }
            |   expr MIN expr           { $$ = createArithOpNode(NODE_SUB, $1, $3); }
            |   expr MULT expr          { $$ = createArithOpNode(NODE_MULT, $1, $3); }
            |   expr DIV expr           { $$ = createArithOpNode(NODE_DIV, $1, $3); }
            |   expr MOD expr           { $$ = createArithOpNode(NODE_MOD, $1, $3); }
            |   expr EQ expr            { $$ = createRelOpNode(NODE_EQ, $1, $3); }
            |   expr NEQ expr           { $$ = createRelOpNode(NODE_NEQ, $1, $3); }
            |   expr GTE expr           { $$ = createRelOpNode(NODE_GTE, $1, $3); }
            |   expr GT expr            { $$ = createRelOpNode(NODE_GT, $1, $3); }
            |   expr LTE expr           { $$ = createRelOpNode(NODE_LTE, $1, $3); }
            |   expr LT expr            { $$ = createRelOpNode(NODE_LT, $1, $3); }
            |   expr AND expr           { $$ = createLogOpNode(NODE_AND, $1, $3); }
            |   expr OR expr            { $$ = createLogOpNode(NODE_OR, $1, $3); }
            |   LPAR expr RPAR          { $$ = $2; }

            |   ID                      { setIdNodeType($1); validateIdForExpr($1); $$ = $1; }
            |   NUMBER                  { $$ = $1; }
            |   ID arrIndex             { setIdNodeType($1); validateArrOffset($1, $2); $1->arrOffset = $2; $$ = $1; }
            |   STR_LTRL                { $$ = $1; }

            |   ID LPAR RPAR            { $$ = createFnCallNode($1, NULL); }
            |   ID LPAR argList RPAR    { $$ = createFnCallNode($1, $3); }

            |   MULT ID                 { $$ = createDerefNode($2); }
            |   AMPSAND ID              { $$ = createAddrToNode($2); }

            |   tupEntry                { $$ = $1; }

            |   NULL_VAL                { $$ = createNullNode(); }
            ;

arrIndex    :   arrIndex LBRACK expr RBRACK { $$ = insertToArrDimn($1, $3); }
            |   LBRACK expr RBRACK          { $$ = $2; }
            ;

argList     :   argList COMMA expr      { $$ = addArgToArgList($1, $3); }
            |   expr                    { $$ = $1; }
            ;

tupEntry    :   ID DOT ID               { setIdNodeType($1); $$ = createTupEntryNode($1, $3); }
            |   ID arrIndex DOT ID      { 
                                            setIdNodeType($1); 
                                            validateArrOffset($1, $2); 
                                            $1->arrOffset = $2; 
                                            $$ = createTupEntryNode($1, $4); 
                                        }
            |   tupEntry DOT ID         { $$ = createTupEntryNode($1, $3); }



%%

int yyerror(const char* s) {
    printf("Error: %s\n", s);
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc > 1)
        yyin = fopen(argv[1], "r");
    else 
        yyin = stdin;

    targetFile = fopen("targetFile.xsm", "w");
    
    setHeader(targetFile);
    resetRegisters();
    createTypeTable();
    
    yyparse();

    exitProgram(targetFile);
    return 0;
}
