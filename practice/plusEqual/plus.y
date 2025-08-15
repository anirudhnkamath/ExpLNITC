%{
	#include <stdio.h>
	#include <stdlib.h>

	int yylex();
	void yyerror(const char* c);
%}

%token PLUS EQ PLUS_EQ ID NUM NEWLINE

%left NEWLINE
%left PLUS
%left EQ PLUS_EQ

%%

start:
	
	ID EQ expr NEWLINE {
		printf("Accepted\n");
		exit(1);
	}|

	ID PLUS_EQ expr NEWLINE {
		printf("Accepted\n");
		exit(1);
	};


expr:

	expr PLUS expr {
	}|

	ID {
	}|

	NUM {
	};

%%

void yyerror(const char* c) {
	printf("Error %s\n", c);
}

int main() {
	yyparse();
	return 1;
}
