%{
#include <iostream>
#include <string>
#include <map>
#include <cstdlib>

using namespace std;

// Symbol Table for Dynamic Variable Storage
map<string, double> varTable;

int yylex();
void yyerror(const char *s);
%}

%union {
    double fval;
    std::string *sval;
}

%token <fval> NUMBER
%token <sval> IDENTIFIER
%token ASSIGN PRINT PLUS MINUS MUL DIV SEMICOLON LPAREN RPAREN

%type <fval> expr term factor

%%

program:
    /* empty */
    | program statement
    ;

statement:
    IDENTIFIER ASSIGN expr SEMICOLON {
        varTable[*$1] = $3;
        cout << "[EXEC] Assigned: " << *$1 << " = " << $3 << "\n";
        delete $1;
    }
    | PRINT expr SEMICOLON {
        cout << "[OUTPUT] " << $2 << "\n";
    }
    ;

expr:
    expr PLUS term { $$ = $1 + $3; }
    | expr MINUS term { $$ = $1 - $3; }
    | term { $$ = $1; }
    ;

term:
    term MUL factor { $$ = $1 * $3; }
    | term DIV factor { 
        if ($3 == 0) {
            cerr << "Error: Division by zero!\n";
            $$ = 0;
        } else {
            $$ = $1 / $3; 
        }
    }
    | factor { $$ = $1; }
    ;

factor:
    NUMBER { $$ = $1; }
    | IDENTIFIER {
        if (varTable.find(*$1) != varTable.end()) {
            $$ = varTable[*$1];
        } else {
            cerr << "Runtime Error: Undefined variable '" << *$1 << "'\n";
            $$ = 0;
        }
        delete $1;
    }
    | LPAREN expr RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    cerr << "Syntax Error: " << s << "\n";
}

int main() {
    cout << "========================================\n";
    cout << "   CONFIG / SCRIPT LANGUAGE EVALUATOR   \n";
    cout << "========================================\n";
    cout << "Type statements (e.g., x = 10; y = x * 2; print y;):\n\n";
    yyparse();
    return 0;
}


sudo apt update && sudo apt install flex bison build-essential -y
bison -d parser.y -o parser.tab.cpp
flex -o lex.yy.cpp lexer.l
g++ parser.tab.cpp lex.yy.cpp -o config_eval -std=c++17
./config_eval

then paste this in terminal 
x = 25;
y = 4;
result = x + y * 2;
print result;
print x / y;