%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}
%%
[ \t\r\n]+              ;   
[0-9]+                  {
                            yyval.num=atoi(yytext);
                            return NUMBER;
                        }

[a-zA-Z_][a-zA-Z0-9_]*  {
                            yylval.str = strdup(yytext);
                            return IDENTIFIER;
                        }
"="                     { return ASSIGN; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MULT; }
"/"                     { return DIV; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
";"                     { return SEMICOLON; }
.                       {
                            printf("Invalid character: %s\n", yytext);
                        }
%%
int yywrap()
{
    return 1;
}
