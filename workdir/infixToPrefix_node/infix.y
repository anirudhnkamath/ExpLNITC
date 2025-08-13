%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "node.h"

	void yyerror(const char* s);
	int yyparse();
	int yylex();
%}

%union {
	struct Node* node;
}

%token <node> WORD
%token PLUS MINUS MULT DIV NEWLINE

%type <node> expr start

%left PLUS MINUS
%left MULT DIV

%%

start : 

	expr NEWLINE {
		$$ = $1;
		preorder($$);
		exit(1);
	};

expr :

	'(' expr ')' {
		$$ = $2;
	}|

	expr PLUS expr {
		$$ = createInternalNode('+', $1, $3);
	}|

	expr MINUS expr {
		$$ = createInternalNode('-', $1, $3);
	}|

	expr MULT expr {
		$$ = createInternalNode('*', $1, $3);
	}|

	expr DIV expr {
		$$ = createInternalNode('/', $1, $3);
	}|

	WORD {
		$$ = $1;
	};

%%

void yyerror(const char* s) {
	printf("Error occured\n");
}

int main() {
	yyparse();
	return 1;
}



