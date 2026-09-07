%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_VARIABLES 100
struct Variable
{
    char name[50];
    int value;
};
struct Variable symbolTable[MAX_VARIABLES];
int variableCount = 0;
int yylex();
void yyerror(const char *s);
int findVariable(char *name)
{
    int i;
    for (i = 0; i < variableCount; i++)
    {
        if (strcmp(symbolTable[i].name, name) == 0)
            return i;
    }
    return -1;
}
void setVariable(char *name, int value)
{
    int index = findVariable(name);

    if (index != -1)
    {
        symbolTable[index].value = value;
    }
    else
    {
        if (variableCount < MAX_VARIABLES)
        {
            strcpy(symbolTable[variableCount].name, name);
            symbolTable[variableCount].value = value;
            variableCount++;
        }
        else
        {
            printf("Error: Symbol table is full.\n");
        }
    }
}
int getVariable(char *name)
{
    int index = findVariable(name);
    if (index == -1)
    {
        printf("Error: Undefined variable '%s'\n", name);
        return 0;
    }

    return symbolTable[index].value;
}
void displayVariables()
{
    int i;
    printf("\n----- Symbol Table -----\n");
    for (i = 0; i < variableCount; i++)
    {
        printf("%s = %d\n",
               symbolTable[i].name,
               symbolTable[i].value);
    }
}
%}
%union
{
    int num;
    char *str;
}
%token <num> NUMBER
%token <str> IDENTIFIER

%token ASSIGN
%token PLUS
%token MINUS
%token MULT
%token DIV
%token LPAREN
%token RPAREN
%token SEMICOLON

%type <num> expression

%left PLUS MINUS
%left MULT DIV
%right UMINUS

%%

program:
        statements
        ;

statements:
        statements statement
        | statement
        ;

statement:
        IDENTIFIER ASSIGN expression SEMICOLON
        {
            setVariable($1, $3);

            printf("%s = %d\n", $1, $3);

            free($1);
        }
        ;

expression:
        NUMBER
        {
            $$ = $1;
        }

        | IDENTIFIER
        {
            $$ = getVariable($1);

            free($1);
        }

        | expression PLUS expression
        {
            $$ = $1 + $3;
        }

        | expression MINUS expression
        {
            $$ = $1 - $3;
        }

        | expression MULT expression
        {
            $$ = $1 * $3;
        }

        | expression DIV expression
        {
            if ($3 == 0)
            {
                printf("Error: Division by zero\n");
                $$ = 0;
            }
            else
            {
                $$ = $1 / $3;
            }
        }

        | MINUS expression %prec UMINUS
        {
            $$ = -$2;
        }

        | LPAREN expression RPAREN
        {
            $$ = $2;
        }
        ;

%%

void yyerror(const char *s)
{
    printf("Syntax Error: %s\n", s);
}

int main()
{
    printf("=====================================\n");
    printf(" Configuration Language Evaluator\n");
    printf("=====================================\n");

    printf("\nEnter statements:\n");
    printf("Example:\n");
    printf("x = 10;\n");
    printf("y = 20;\n");
    printf("z = x + y;\n");
    printf("result = z * 2;\n\n");

    yyparse();

    displayVariables();

    return 0;
}
