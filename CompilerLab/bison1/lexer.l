%{
#include <string>
#include "parser.tab.hpp"
extern "C" int yylex();
%}

%option noyywrap

%%

[ \t\n]+            ; // Skip whitespace
[0-9]+(\.[0-9]+)?   { yylval.fval = atof(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = new std::string(yytext); return IDENTIFIER; }
"="                 { return ASSIGN; }
"+"                 { return PLUS; }
"-"                 { return MINUS; }
"*"                 { return MUL; }
"/"                 { return DIV; }
"print"             { return PRINT; }
";"                 { return SEMICOLON; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }
.                   { return yytext[0]; }

%%


syntax checkar and evaluator 