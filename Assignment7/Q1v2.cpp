#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cctype>
using namespace std;

// ============================================================
// 1. TOKEN
// ============================================================

enum TokenType
{
    ID,
    INT_NUM,
    FLOAT_NUM,
    TRUE_TOKEN,
    FALSE_TOKEN,

    PLUS,
    MINUS,
    MUL,
    DIV,
    ASSIGN,

    LPAREN,
    RPAREN,

    END
};

struct Token
{
    TokenType type;
    string value;

    Token(TokenType t, string v)
    {
        type = t;
        value = v;
    }
};


// ============================================================
// 2. LEXER
// ============================================================

class Lexer
{
    string input;
    int pos;

public:

    Lexer(string s)
    {
        input = s;
        pos = 0;
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;

        while (pos < input.length())
        {
            // Ignore spaces
            if (isspace(input[pos]))
            {
                pos++;
                continue;
            }

            // Identifier
            if (isalpha(input[pos]))
            {
                string word;

                while (pos < input.length() &&
                       isalnum(input[pos]))
                {
                    word += input[pos];
                    pos++;
                }

                if (word == "true")
                    tokens.push_back(
                        Token(TRUE_TOKEN, word));

                else if (word == "false")
                    tokens.push_back(
                        Token(FALSE_TOKEN, word));

                else
                    tokens.push_back(
                        Token(ID, word));

                continue;
            }

            // Number
            if (isdigit(input[pos]))
            {
                string number;
                bool isFloat = false;

                while (pos < input.length() &&
                       (isdigit(input[pos]) ||
                        input[pos] == '.'))
                {
                    if (input[pos] == '.')
                        isFloat = true;

                    number += input[pos];
                    pos++;
                }

                if (isFloat)
                    tokens.push_back(
                        Token(FLOAT_NUM, number));
                else
                    tokens.push_back(
                        Token(INT_NUM, number));

                continue;
            }

            // Operators and brackets
            if (input[pos] == '+')
            {
                tokens.push_back(Token(PLUS, "+"));
                pos++;
            }

            else if (input[pos] == '-')
            {
                tokens.push_back(Token(MINUS, "-"));
                pos++;
            }

            else if (input[pos] == '*')
            {
                tokens.push_back(Token(MUL, "*"));
                pos++;
            }

            else if (input[pos] == '/')
            {
                tokens.push_back(Token(DIV, "/"));
                pos++;
            }

            else if (input[pos] == '=')
            {
                tokens.push_back(Token(ASSIGN, "="));
                pos++;
            }

            else if (input[pos] == '(')
            {
                tokens.push_back(Token(LPAREN, "("));
                pos++;
            }

            else if (input[pos] == ')')
            {
                tokens.push_back(Token(RPAREN, ")"));
                pos++;
            }

            else
            {
                cout << "Lexical Error: Invalid character '"
                     << input[pos] << "'\n";

                pos++;
            }
        }

        tokens.push_back(Token(END, "$"));

        return tokens;
    }
};


// ============================================================
// 3. TYPES
// ============================================================

enum Type
{
    INT,
    FLOAT,
    BOOL,
    ERROR
};

string typeName(Type type)
{
    if (type == INT)
        return "int";

    if (type == FLOAT)
        return "float";

    if (type == BOOL)
        return "bool";

    return "error";
};


// ============================================================
// 4. AST BASE CLASS
// ============================================================

class ASTNode
{
public:

    virtual void print(int level = 0) = 0;

    virtual Type checkType(
        map<string, Type>& symbolTable) = 0;

    virtual ~ASTNode() {}
};


// ============================================================
// 5. AST - LITERAL
// ============================================================

class Literal : public ASTNode
{
    string value;
    Type type;

public:

    Literal(string v, Type t)
    {
        value = v;
        type = t;
    }

    void print(int level = 0)
    {
        for (int i = 0; i < level; i++)
            cout << "  ";

        cout << value << " : "
             << typeName(type) << endl;
    }

    Type checkType(
        map<string, Type>& symbolTable)
    {
        return type;
    }
};


// ============================================================
// 6. AST - VARIABLE
// ============================================================

class Variable : public ASTNode
{
    string name;

public:

    Variable(string n)
    {
        name = n;
    }

    void print(int level = 0)
    {
        for (int i = 0; i < level; i++)
            cout << "  ";

        cout << name << endl;
    }

    Type checkType(
        map<string, Type>& symbolTable)
    {
        if (symbolTable.find(name) ==
            symbolTable.end())
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' is not declared.\n";

            return ERROR;
        }

        return symbolTable[name];
    }
};


// ============================================================
// 7. AST - BINARY EXPRESSION
// ============================================================

class BinaryExpr : public ASTNode
{
    string op;

    ASTNode* left;
    ASTNode* right;

public:

    BinaryExpr(
        string o,
        ASTNode* l,
        ASTNode* r)
    {
        op = o;
        left = l;
        right = r;
    }

    void print(int level = 0)
    {
        for (int i = 0; i < level; i++)
            cout << "  ";

        cout << op << endl;

        left->print(level + 1);
        right->print(level + 1);
    }

    Type checkType(
        map<string, Type>& symbolTable)
    {
        Type leftType =
            left->checkType(symbolTable);

        Type rightType =
            right->checkType(symbolTable);

        if (leftType == ERROR ||
            rightType == ERROR)
        {
            return ERROR;
        }

        // Arithmetic operators
        if (op == "+" ||
            op == "-" ||
            op == "*" ||
            op == "/")
        {
            // Both operands must be numeric
            if ((leftType == INT ||
                 leftType == FLOAT) &&
                (rightType == INT ||
                 rightType == FLOAT))
            {
                // int + float = float
                if (leftType == FLOAT ||
                    rightType == FLOAT)
                {
                    return FLOAT;
                }

                return INT;
            }

            cout << "Semantic Error: Cannot perform '"
                 << op
                 << "' on "
                 << typeName(leftType)
                 << " and "
                 << typeName(rightType)
                 << ".\n";

            return ERROR;
        }

        return ERROR;
    }
};


// ============================================================
// 8. AST - ASSIGNMENT
// ============================================================

class Assignment : public ASTNode
{
    string name;
    ASTNode* expression;

public:

    Assignment(
        string n,
        ASTNode* e)
    {
        name = n;
        expression = e;
    }

    void print(int level = 0)
    {
        for (int i = 0; i < level; i++)
            cout << "  ";

        cout << "=" << endl;

        for (int i = 0; i < level + 1; i++)
            cout << "  ";

        cout << name << endl;

        expression->print(level + 1);
    }

    Type checkType(
        map<string, Type>& symbolTable)
    {
        // Check left-hand variable
        if (symbolTable.find(name) ==
            symbolTable.end())
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' is not declared.\n";

            return ERROR;
        }

        Type variableType =
            symbolTable[name];

        // Check right-hand expression
        Type expressionType =
            expression->checkType(symbolTable);

        if (expressionType == ERROR)
            return ERROR;

        // Check assignment compatibility
        if (variableType != expressionType)
        {
            cout << "Semantic Error: Cannot assign "
                 << typeName(expressionType)
                 << " to variable '"
                 << name
                 << "' of type "
                 << typeName(variableType)
                 << ".\n";

            return ERROR;
        }

        return variableType;
    }
};


// ============================================================
// 9. RECURSIVE-DESCENT PARSER
//
// Grammar:
//
// assignment  -> ID = expression
//
// expression  -> term { (+ | -) term }
//
// term        -> factor { (* | /) factor }
//
// factor      -> ID
//             | INT_NUM
//             | FLOAT_NUM
//             | true
//             | false
//             | ( expression )
//
// ============================================================

class Parser
{
    vector<Token> tokens;
    int pos;

public:

    Parser(vector<Token> t)
    {
        tokens = t;
        pos = 0;
    }

    Token current()
    {
        return tokens[pos];
    }

    void error(string message)
    {
        cout << "Syntax Error: "
             << message << endl;
    }

    // assignment -> ID = expression
    ASTNode* parseAssignment()
    {
        if (current().type != ID)
        {
            error("Expected variable.");
            return NULL;
        }

        string name = current().value;
        pos++;

        if (current().type != ASSIGN)
        {
            error("Expected '='.");
            return NULL;
        }

        pos++;

        ASTNode* expression =
            parseExpression();

        if (expression == NULL)
            return NULL;

        if (current().type != END)
        {
            error("Unexpected token '" +
                  current().value + "'.");
            return NULL;
        }

        return new Assignment(
            name,
            expression);
    }

    // expression -> term { (+ | -) term }
    ASTNode* parseExpression()
    {
        ASTNode* left =
            parseTerm();

        if (left == NULL)
            return NULL;

        while (current().type == PLUS ||
               current().type == MINUS)
        {
            string op = current().value;
            pos++;

            ASTNode* right =
                parseTerm();

            if (right == NULL)
                return NULL;

            left =
                new BinaryExpr(
                    op,
                    left,
                    right);
        }

        return left;
    }

    // term -> factor { (* | /) factor }
    ASTNode* parseTerm()
    {
        ASTNode* left =
            parseFactor();

        if (left == NULL)
            return NULL;

        while (current().type == MUL ||
               current().type == DIV)
        {
            string op = current().value;
            pos++;

            ASTNode* right =
                parseFactor();

            if (right == NULL)
                return NULL;

            left =
                new BinaryExpr(
                    op,
                    left,
                    right);
        }

        return left;
    }

    // factor -> operand | ( expression )
    ASTNode* parseFactor()
    {
        Token token = current();

        // Variable
        if (token.type == ID)
        {
            pos++;

            return new Variable(
                token.value);
        }

        // Integer
        if (token.type == INT_NUM)
        {
            pos++;

            return new Literal(
                token.value,
                INT);
        }

        // Float
        if (token.type == FLOAT_NUM)
        {
            pos++;

            return new Literal(
                token.value,
                FLOAT);
        }

        // Boolean true
        if (token.type == TRUE_TOKEN)
        {
            pos++;

            return new Literal(
                "true",
                BOOL);
        }

        // Boolean false
        if (token.type == FALSE_TOKEN)
        {
            pos++;

            return new Literal(
                "false",
                BOOL);
        }

        // Parentheses
        if (token.type == LPAREN)
        {
            pos++;

            ASTNode* node =
                parseExpression();

            if (current().type != RPAREN)
            {
                error("Expected ')'.");
                return NULL;
            }

            pos++;

            return node;
        }

        error("Invalid expression.");
        return NULL;
    }
};


// ============================================================
// 10. MAIN
// ============================================================

int main()
{
    map<string, Type> symbolTable;

    cout << "=====================================\n";
    cout << "     SEMANTIC ANALYZER USING AST\n";
    cout << "=====================================\n\n";


    // --------------------------------------------------------
    // STEP 1: READ DECLARATIONS
    // --------------------------------------------------------

    int n;

    cout << "Enter number of declarations: ";
    cin >> n;

    cout << "\nEnter declarations:\n";
    cout << "Example: a int\n\n";

    for (int i = 0; i < n; i++)
    {
        string name;
        string type;

        cin >> name >> type;

        if (symbolTable.find(name) !=
            symbolTable.end())
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' already declared.\n";

            continue;
        }

        if (type == "int")
            symbolTable[name] = INT;

        else if (type == "float")
            symbolTable[name] = FLOAT;

        else if (type == "bool")
            symbolTable[name] = BOOL;

        else
        {
            cout << "Invalid type: "
                 << type << endl;

            i--;
        }
    }


    // --------------------------------------------------------
    // DISPLAY SYMBOL TABLE
    // --------------------------------------------------------

    cout << "\nSymbol Table:\n";
    cout << "-------------\n";

    for (auto x : symbolTable)
    {
        cout << x.first
             << " : "
             << typeName(x.second)
             << endl;
    }


    // --------------------------------------------------------
    // STEP 2: READ SOURCE EXPRESSION
    // --------------------------------------------------------

    cin.ignore();

    string input;

    cout << "\nEnter assignment expression:\n";
    cout << "Example: b = a + 5\n\n";

    getline(cin, input);


    // --------------------------------------------------------
    // STEP 3: LEXICAL ANALYSIS
    // --------------------------------------------------------

    Lexer lexer(input);

    vector<Token> tokens =
        lexer.tokenize();

    cout << "\nTokens:\n";
    cout << "-------\n";

    for (Token token : tokens)
    {
        if (token.type != END)
        {
            cout << "[" << token.value << "] ";
        }
    }

    cout << endl;


    // --------------------------------------------------------
    // STEP 4: PARSING
    // --------------------------------------------------------

    Parser parser(tokens);

    ASTNode* ast =
        parser.parseAssignment();

    if (ast == NULL)
    {
        cout << "\nParsing failed.\n";
        return 0;
    }

    cout << "\nParsing successful.\n";


    // --------------------------------------------------------
    // STEP 5: DISPLAY AST
    // --------------------------------------------------------

    cout << "\nAbstract Syntax Tree:\n";
    cout << "---------------------\n";

    ast->print();


    // --------------------------------------------------------
    // STEP 6: SEMANTIC ANALYSIS
    // --------------------------------------------------------

    cout << "\nSemantic Analysis:\n";
    cout << "------------------\n";

    Type result =
        ast->checkType(symbolTable);

    if (result != ERROR)
    {
        cout << "No semantic errors found.\n";
        cout << "Expression is type correct.\n";
    }


    return 0;
}
