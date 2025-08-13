%{
	#include <string.h>
	#include "y.tab.h"
	#include "node.h"
%}

character [a-zA-Z]

%%

{character}+ {
	char* strbuf = strdup(yytext);
	yylval.node = createLeafNode(strbuf);
	return WORD;
}

"+" {
	return PLUS;
}

"-" {
	return MINUS;
}

"*" {
	return MULT;
}

"/" {
	return DIV;
}

[\(\)] {
	return *yytext;
}

\n {
	return NEWLINE;
}

[ \t]+ {
}

. {
	return -1;
}

%% 

int yywrap() {
	return 1;
}
