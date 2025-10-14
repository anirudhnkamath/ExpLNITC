%{
    #include "./label/label.h"
    #include <stdio.h>

    int addr = 2056;
    int scanNumber = 1;

    FILE* translatedFile;
%}

label_name (L[0-9]+|F[0-9]+|MAIN)
number [0-9]+
register (R[0-9]+)

jz_instr (JZ[ ]{register},[ ]{label_name})
jnz_instr (JNZ[ ]{register},[ ]{label_name})
jmp_instr (JMP[ ]{label_name})
call_instr (CALL[ ]{label_name})
main_header MAIN

%%

{main_header}\n {
    if(scanNumber == 1) {}
    else {
        int addr = findLabelAddr("MAIN");
        fprintf(translatedFile, "%d\n", addr);
    }
}

{number}\n {
    if(scanNumber == 1) {}
    else {
        fprintf(translatedFile, "%s", yytext);
    }
}

{label_name}:\n {
    if(scanNumber == 1) {
        char newLabel[yyleng];
        strcpy(newLabel, yytext);
        newLabel[yyleng-1] = newLabel[yyleng-2] = '\0';
        insertNewLabel(newLabel, addr);
    }
    else {}
}

{jz_instr}\n {
    if(scanNumber == 1) {
        addr += 2;
    }
    else {
        char curLabel[10], curReg[10];
        sscanf(yytext, "JZ %s %s\n", curReg, curLabel);

        int addr = findLabelAddr(curLabel);
        fprintf(translatedFile, "JZ %s %d\n", curReg, addr);
    }
}

{jnz_instr}\n {
    if(scanNumber == 1) {
        addr += 2;
    }
    else {
        char curLabel[10], curReg[10];
        sscanf(yytext, "JNZ %s %s\n", curReg, curLabel);

        int addr = findLabelAddr(curLabel);
        fprintf(translatedFile, "JNZ %s %d\n", curReg, addr);
    }
}

{jmp_instr}\n {
    if(scanNumber == 1) {
        addr += 2;
    }
    else {
        char curLabel[10];
        sscanf(yytext, "JMP %s\n", curLabel);

        int addr = findLabelAddr(curLabel);
        fprintf(translatedFile, "JMP %d\n", addr);
    }
}

{call_instr}\n {
    if(scanNumber == 1) {
        addr += 2;
    }
    else {
        char curLabel[10];
        sscanf(yytext, "CALL %s\n", curLabel);

        int addr = findLabelAddr(curLabel);
        fprintf(translatedFile, "CALL %d\n", addr);
    }
}

.*\n {
    if(scanNumber == 1) {
        addr += 2;
    }
    else {
        fprintf(translatedFile, "%s", yytext);
    }
}

%%

int yywrap() {
    if(scanNumber == 1) {
        scanNumber = 2;
        rewind(yyin);
        return 0;
    }
    else {
        return 1;
    }
}

int main(int argc, char* argv[]) {
    if(argc > 1)
        yyin = fopen(argv[1], "r");
    else 
        yyin = stdin;

    translatedFile = fopen("translatedFile.xsm", "w");

    yylex();
}

