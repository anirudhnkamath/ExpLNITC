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

    PLUS expr expr {
        $$ = createInternalNode('+', $2, $3);
    }|

    MIN expr expr {
        $$ = createInternalNode('-', $2, $3);
    }|

    MULT expr expr {
        $$ = createInternalNode('*', $2, $3);
    }|

    DIV expr expr {
        $$ = createInternalNode('/', $2, $3);
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