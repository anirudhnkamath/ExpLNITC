set -e 

if [ $# -ne 1 ]; then
    echo "Enter file"
    exit 1
fi

INPUT_FILE=$1

lex lex.l
yacc -d yacc.y

gcc -o main_compiler \
    -g lex.yy.c y.tab.c \
    codeGen/codeGen.c \
    node/node.c \
    registers/registers.c \
    gsTable/gsTable.c \
    label/label.c \
    typeTable/typeTable.c \
    xsmGen/xsmGen.c

# compiler to generate labelled code
gdb --args main_compiler "$INPUT_FILE"
