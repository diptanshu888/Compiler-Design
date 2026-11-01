%{
#include <stdio.h>
#include <string.h>
%}

%%

"int"|"float"|"double"|"char"|"if"|"else"|"while"|"for"|"return"|"void" { 
    printf("%-22s %-20s\n", "KEYWORD", yytext); 
}

[a-zA-Z_][a-zA-Z0-9_]* { 
    printf("%-22s %-20s\n", "IDENTIFIER", yytext); 
}

[0-9]+(\.[0-9]+)? { 
    printf("%-22s %-20s\n", "NUMBER", yytext); 
}

"=="|"<="|">="|"!=" { 
    printf("%-22s %-20s\n", "OPERATOR", yytext); 
}

[\+\-\*/=<>\(\)\{\};,] { 
    printf("%-22s %-20s\n", "DELIMITER", yytext); 
}

[ \t\n]+ { /* ignore whitespace */ }

. { 
    printf("%-22s %-20s\n", "UNKNOWN", yytext); 
}

%%

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            perror(argv[1]);
            return 1;
        }
        yyin = f;
    }
    printf("%-22s %-20s\n", "TOKEN TYPE", "LEXEME");
    printf("------------------------------------------\n");
    yylex();
    printf("------------------------------------------\n");
    return 0;
}

int yywrap() {
    return 1;
}

flex lexer.l
gcc lex.yy.c -lfl -o lexer
./lexer input.txt