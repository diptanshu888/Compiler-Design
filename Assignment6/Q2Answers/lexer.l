%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%%

[ \t\r\n]+          ;

[0-9]+              {
                        yylval.num = atoi(yytext);
                        return NUMBER;
                    }

"+"                 { return PLUS; }
"-"                 { return MINUS; }
"*"                 { return MULT; }
"/"                 { return DIV; }

"("                 { return LPAREN; }
")"                 { return RPAREN; }

.                   {
                        printf("Lexical Error: Invalid character '%s'\n", yytext);
                    }

%%

int yywrap()
{
    return 1;
}
