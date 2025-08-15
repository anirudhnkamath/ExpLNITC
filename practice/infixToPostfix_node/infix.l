%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "y.tab.h"
	#include "node.h"
%}

digit [0-9]

%%

{digit}+ {
	int num = atoi(yytext);
	yylval.node = makeLeafNode(num);
	return NUMBER;
}

\+ {
	return PLUS;
}

\- {
	return MINUS;
}

\* {
	return MULT;
}

\/ {
	return DIV;
}

[\(\)] {
	return *yytext;
}

[\n] {
	return NEWLINE;
}

[ \s\t] {
}

. {
	return -1;
}


%%

int yywrap() {
	return 1;
}
