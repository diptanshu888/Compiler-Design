%{
#include <string>
#include "parser.tab.hpp"
extern "C" int yylex();
%}

%option noyywrap

%%

[ \t\n]+            ; // Skip whitespaces
[0-9]+(\.[0-9]+)?   { yylval.fval = atof(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = new std::string(yytext); return IDENTIFIER; }
"+"                 { return PLUS; }
"-"                 { return MINUS; }
"*"                 { return MUL; }
"/"                 { return DIV; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }
";"                 { return SEMICOLON; }
.                   { return yytext[0]; }

%%