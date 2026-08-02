%{
#include <stdio.h>

FILE *yyin;
%}

%%

(int|float|char|if|else|while|for|return) { printf("Keyword    : %s\n", yytext); }

[a-zA-Z_][a-zA-Z0-9_]* { printf("Identifier : %s\n", yytext); }

[0-9]+ { printf("Number     : %s\n", yytext); }

("=="|"!="|"<="|">="|"<"|">"|"="|"+"|"-"|"*"|"/") { printf("Operator   : %s\n", yytext); }

(";"|"("|")"|"{"|"}"|",") { printf("Delimiter  : %s\n", yytext); }

[ \t\n]+ ;

. { printf("Unknown    : %s\n", yytext); }

%%

int yywrap()
{
    return 1;
}

int main()
{
    FILE *fp;

    fp = fopen("input.c", "w");

    if(fp == NULL)
    {
        printf("Cannot create file\n");
        return 1;
    }

    fprintf(fp, "int a = 10;\n");
    fprintf(fp, "float b = 20;\n");
    fprintf(fp, "if(a<b)\n");
    fprintf(fp, "{\n");
    fprintf(fp, "a=a+1;\n");
    fprintf(fp, "}\n");

    fclose(fp);

    yyin = fopen("input.c", "r");

    if(yyin == NULL)
    {
        printf("Cannot open file\n");
        return 1;
    }

    printf("Tokens:\n\n");

    yylex();

    fclose(yyin);

    return 0;
}