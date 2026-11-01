%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);

int semanticError = 0;
%}

%union
{
    int num;
}

%token <num> NUMBER

%token PLUS
%token MINUS
%token MULT
%token DIV

%token LPAREN
%token RPAREN

%type <num> expression

/* Operator precedence */
%left PLUS MINUS
%left MULT DIV
%right UMINUS

%%

program:
        expression
        {
            if (semanticError == 0)
            {
                printf("\nValid expression.\n");
                printf("Result = %d\n", $1);
            }
            else
            {
                printf("\nExpression contains semantic errors.\n");
            }
        }
        ;

expression:
        NUMBER
        {
            $$ = $1;
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
                printf("Semantic Error: Division by zero.\n");
                semanticError = 1;
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
    printf(" Mathematical Expression Validator\n");
    printf("=====================================\n");

    printf("\nEnter a mathematical expression:\n");
    printf("Example: (10 + 20) * (5 - 2)\n\n");

    yyparse();

    return 0;
}
