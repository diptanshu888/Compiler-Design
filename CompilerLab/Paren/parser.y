%{
#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <algorithm>

using namespace std;

// Symbol Table for variable evaluation
map<string, double> symbolTable;

// Parenthesis Nesting Metrics & Validators
int current_depth = 0;
int max_depth = 0;

int yylex();
void yyerror(const char *s);
%}

%union {
    double fval;
    std::string *sval;
}

%token <fval> NUMBER
%token <sval> IDENTIFIER
%token PLUS MINUS MUL DIV LPAREN RPAREN SEMICOLON

%type <fval> expr term factor

%%

program:
    /* empty */
    | program statement
    ;

statement:
    expr SEMICOLON {
        cout << "--------------------------------------------------------\n";
        cout << " [STATUS] Syntactically and Structurally Valid.\n";
        cout << " [METRICS] Max Parenthesis Nesting Depth: " << max_depth << "\n";
        cout << " [EVAL] Result = " << $1 << "\n";
        cout << "--------------------------------------------------------\n\n";
        current_depth = 0;
        max_depth = 0;
    }
    | error SEMICOLON {
        yyerrok;
        current_depth = 0;
        max_depth = 0;
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
            cerr << " [SEMANTIC ERROR] Division by zero detected!\n";
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
        if (symbolTable.find(*$1) != symbolTable.end()) {
            $$ = symbolTable[*$1];
        } else {
            cerr << " [SEMANTIC WARNING] Undefined variable '" << *$1 << "', defaulting to 0.\n";
            $$ = 0;
        }
        delete $1;
    }
    | LPAREN {
        current_depth++;
        if (current_depth > max_depth) {
            max_depth = current_depth;
        }
    } expr RPAREN {
        current_depth--;
        $$ = $3;
    }
    ;

%%

void yyerror(const char *s) {
    cerr << " [SYNTAX/BALANCE ERROR] " << s << " (Check parenthesis matching or expression structure)\n";
}

int main() {
    // Pre-populate sample symbol table
    symbolTable["x"] = 12.0;
    symbolTable["y"] = 4.0;

    cout << "========================================================\n";
    cout << "     NESTED PARENTHESIS & MATH STRUCTURAL VALIDATOR     \n";
    cout << "========================================================\n";
    cout << " Preloaded variables: x = 12.0, y = 4.0\n";
    cout << " Type math expressions ending with ';' (Press Ctrl+C to exit):\n\n";
    
    yyparse();
    return 0;
}


sudo apt update && sudo apt install flex bison build-essential -y
bison -d parser.y -o parser.tab.cpp
flex -o lex.yy.cpp lexer.l
g++ parser.tab.cpp lex.yy.cpp -o struct_validator -std=c++17
./struct_validator

((x + y) * (5 - 2)) / 2;

