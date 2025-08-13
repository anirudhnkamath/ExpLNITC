%{
	#include "y.tab.h"
%}

digit [0-9]
letter [a-zA-Z]

%%

{digit}+ {
	return NUM;
}

{letter}({letter}|{digit})* {
	return ID;
}

"+=" {
	return PLUS_EQ;
}

"+" {
	return PLUS;
}

"=" {
	return EQ;
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

