%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "node.h"

	int yylex();
	void yyerror();
%}

%union {
	struct Node* node;
};

%token <node> NUMBER
%token NEWLINE PLUS MINUS MULT DIV

%type <node> expr start

%left PLUS MINUS
%left MULT DIV

%%

start :
	
	expr NEWLINE {
		printf("Completed, value = %d\n", evaluateTree($1));
		exit(1);
	};

expr :

	'(' expr ')' {
		$$ = $2;
	}|

	expr PLUS expr {
		$$ = makeOperatorNode('+', $1, $3);
	}|

	expr MINUS expr {
		$$ = makeOperatorNode('-', $1, $3);
	}|

	expr DIV expr {
		$$ = makeOperatorNode('/', $1, $3);
	}|

	expr MULT expr {
		$$ = makeOperatorNode('*', $1, $3);
	}|

	NUMBER {
		$$ = $1;
	};

%%

void yyerror(const char* s) {
	printf("\nInvalid\n");
}

int main() {
	yyparse();
	return 1;
}
