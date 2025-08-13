%{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>

	int nestLevel = 0;

	int foundEnd = 0;
%}

%token CMD IF ELSE COND END

%%

start :

	command END {
		printf("Good syntax.\n");
		exit(0);
	};

command :

	CMD {
	}|

	IF COND command ELSE command {
	};

%%

int yyerror(const char* s) {
	printf("Syntax error in your language.\n");
	return 0;
}

int yylex() {

	if(foundEnd)
		return END;

	char curInput[10] = "";
	int index = 0;

	char c = getchar();
	while(c != ' ' && c != '\n') {
		curInput[index] = c;
		index += 1;
		c = getchar();
	}

	if(c == '\n')
		foundEnd = 1;

	if(strcmp(curInput, "if") == 0) 
		return IF;

	if(strcmp(curInput, "cmd") == 0) 
		return CMD;

	if(strcmp(curInput, "cond") == 0) 
		return COND;

	if(strcmp(curInput, "else") == 0)
		return ELSE;

	return END;

}

int main() {
	yyparse();
	return 0;
}




