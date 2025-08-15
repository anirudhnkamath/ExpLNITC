%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void to_upper(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}
%}

%%

[A][a-zA-Z]{2} {
    if (strlen(yytext) == 3 && yytext[0] == 'A') {
        to_upper(yytext);
        printf("%s\n", yytext);
    }
}

[a-zA-Z]+ {

}

.|\n {
}

%%

int yywrap() {
    return 1;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            perror("File open failed");
            return 1;
        }
        yyin = fp;
    } else {
        yyin = stdin;
    }

    yylex();
    return 0;
}
