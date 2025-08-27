%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "./registers/registers.h"
    #include "./codeGen/codeGen.h"

    int yylex(void);
    int yyerror(const char *s);
%}

%union {
    struct Tnode* node;
}

%token <node> NUMBER 
%token NEWLINE PLUS MIN MULT DIV LBRACE RBRACE

%type <node> start expr

%left PLUS MIN
%left MULT DIV

%%

start : 

    expr NEWLINE {
        $$ = $1;

        FILE* filePtr = fopen("target_file.xsm", "w");

        resetRegisters();
        setHeader(filePtr);
        reg_index_t regIndex = codeGen($$, filePtr);
        printToConsole(regIndex, filePtr);
        exitProgram(filePtr);

        fclose(filePtr);

        exit(0);
    };

expr :

    LBRACE expr RBRACE {
        $$ = $2;
    }| 

    expr PLUS expr {
        $$ = createInternalNode('+', $1, $3);
    }|

    expr MIN expr {
        $$ = createInternalNode('-', $1, $3);
    }|

    expr MULT expr {
        $$ = createInternalNode('*', $1, $3);
    }|

    expr DIV expr {
        $$ = createInternalNode('/', $1, $3);
    }|

    NUMBER {
        $$ = $1;
    };

%%

int yyerror(const char* s) {
    printf("Error occured\n");
    return 0;
}

int main() {
    yyparse();
    return 0;
}