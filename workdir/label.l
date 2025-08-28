%{
    #include "./label/label.h"
    #include <stdio.h>

    int addr = 2056;
%}

%%

[0-9]+\n {}

[LF][0-9]+:\n {
    char newLabel[yyleng];
    strcpy(newLabel, yytext);
    newLabel[yyleng-1] = newLabel[yyleng-2] = '\0';

    insertNewLabel(newLabel, addr);
}

MAIN:\n {
    char newLabel[yyleng];
    strcpy(newLabel, yytext);
    newLabel[yyleng-1] = newLabel[yyleng-2] = '\0';

    insertNewLabel(newLabel, addr);
}

.*\n {
    addr += 2;
}

%%

int yywrap() {
    return 1;
}

int main() {
    yylex();

    Label* temp = labelHead;
    while(temp) {
        printf("%s %d\n", temp->labelName, temp->addr);
        temp = temp->next;
    }
}

