#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
using namespace std;

// ---------------- TYPE ----------------

enum Type
{
    INT,
    FLOAT,
    BOOL,
    ERROR
};

string typeName(Type t)
{
    if (t == INT) return "int";
    if (t == FLOAT) return "float";
    if (t == BOOL) return "bool";
    return "error";
}

// ---------------- AST NODE ----------------

struct Node
{
    virtual Type checkType(map<string, Type>& symbolTable) = 0;
    virtual void print(int level = 0) = 0;
    virtual ~Node() {}
};

// ---------------- LITERAL ----------------

struct Literal : Node
{
    Type type;
    string value;

    Literal(string v, Type t)
    {
        value = v;
        type = t;
    }

    Type checkType(map<string, Type>& symbolTable)
    {
        return type;
    }

    void print(int level = 0)
    {
        for (int i = 0; i < level; i++)
            cout << "  ";

        cout << value << " : " << typeName(type) << endl;
    }
};

// ---------------- VARIABLE ----------------

struct Variable : Node
{
    string name;

    Variable(string n)
    {
        name = n;
    }

    Type checkType(map<string, Type>& symbolTable)
    {
        if (symbolTable.find(name) == symbolTable.end())
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' is not declared.\n";

            return ERROR;
        }

        return symbolTable[name];
    }

    void print(int level = 0)
    {
        for (int i = 0; i < level; i++)
            cout << "  ";

        cout << name << endl;
    }
};

// ---------------- BINARY EXPRESSION ----------------

struct BinaryExpr : Node
{
    string op;
    Node* left;
    Node* right;

    BinaryExpr(string o, Node* l, Node* r)
    {
        op = o;
        left = l;
        right = r;
    }

    Type checkType(map<string, Type>& symbolTable)
    {
        Type leftType = left->checkType(symbolTable);
        Type rightType = right->checkType(symbolTable);

        if (leftType == ERROR || rightType == ERROR)
            return ERROR;

        // Arithmetic operations
        if (op == "+" || op == "-" ||
            op == "*" || op == "/")
        {
            if ((leftType == INT || leftType == FLOAT) &&
                (rightType == INT || rightType == FLOAT))
            {
                if (leftType == FLOAT ||
                    rightType == FLOAT)
                    return FLOAT;

                return INT;
            }

            cout << "Semantic Error: Invalid operands for '"
                 << op << "'.\n";

            return ERROR;
        }

        return ERROR;
    }

    void print(int level = 0)
    {
        for (int i = 0; i < level; i++)
            cout << "  ";

        cout << op << endl;

        left->print(level + 1);
        right->print(level + 1);
    }
};

// ---------------- ASSIGNMENT ----------------

struct Assignment : Node
{
    string name;
    Node* expression;

    Assignment(string n, Node* e)
    {
        name = n;
        expression = e;
    }

    Type checkType(map<string, Type>& symbolTable)
    {
        // Check whether variable is declared
        if (symbolTable.find(name) == symbolTable.end())
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' is not declared.\n";

            return ERROR;
        }

        Type variableType = symbolTable[name];

        // Check expression
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
};

// ---------------- CREATE OPERAND ----------------

Node* createOperand(string token)
{
    // Integer
    bool isInt = true;

    for (char c : token)
    {
        if (c < '0' || c > '9')
        {
            isInt = false;
            break;
        }
    }

    if (isInt && !token.empty())
        return new Literal(token, INT);

    // Float
    bool isFloat = false;
    int dots = 0;

    for (char c : token)
    {
        if (c == '.')
        {
            dots++;
            isFloat = true;
        }
        else if (c < '0' || c > '9')
        {
            isFloat = false;
            break;
        }
    }

    if (isFloat && dots == 1)
        return new Literal(token, FLOAT);

    // Otherwise assume it is a variable
    return new Variable(token);
}

// ---------------- MAIN ----------------

int main()
{
    map<string, Type> symbolTable;

    cout << "===== SEMANTIC ANALYZER =====\n\n";

    // ---------------- DECLARATIONS ----------------

    int n;

    cout << "Enter number of declarations: ";
    cin >> n;

    cout << "\nEnter declarations:\n";
    cout << "Example: a int\n\n";

    for (int i = 0; i < n; i++)
    {
        string name, type;

        cin >> name >> type;

        if (type == "int")
            symbolTable[name] = INT;

        else if (type == "float")
            symbolTable[name] = FLOAT;

        else if (type == "bool")
            symbolTable[name] = BOOL;

        else
        {
            cout << "Invalid type.\n";
            i--;
        }
    }

    // ---------------- ASSIGNMENT INPUT ----------------

    cin.ignore();

    string input;

    cout << "\nEnter assignment expression:\n";
    cout << "Example: b = a + 5\n";

    getline(cin, input);

    // Remove spaces
    stringstream ss(input);

    vector<string> tokens;
    string token;

    while (ss >> token)
        tokens.push_back(token);

    // ---------------- PARSE ----------------

    if (tokens.size() < 3 || tokens[1] != "=")
    {
        cout << "\nInvalid assignment expression.\n";
        return 0;
    }

    string variableName = tokens[0];

    Node* expression = NULL;

    // expression: operand
    if (tokens.size() == 3)
    {
        expression = createOperand(tokens[2]);
    }

    // expression: operand operator operand
    else if (tokens.size() == 5)
    {
        string leftToken = tokens[2];
        string op = tokens[3];
        string rightToken = tokens[4];

        Node* left = createOperand(leftToken);
        Node* right = createOperand(rightToken);

        expression =
            new BinaryExpr(op, left, right);
    }

    else
    {
        cout << "\nExpression format not supported.\n";
        cout << "Use: a = 10\n";
        cout << "or:  a = b + 5\n";
        return 0;
    }

    // ---------------- BUILD AST ----------------

    Node* ast =
        new Assignment(variableName, expression);

    // ---------------- DISPLAY AST ----------------

    cout << "\nAbstract Syntax Tree:\n";
    cout << "---------------------\n";

    ast->print();

    // ---------------- SEMANTIC ANALYSIS ----------------

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
}        }

        // Comparison operators
        if (op == ">" || op == "<" ||
            op == "==" || op == "!=")
        {
            if ((leftType == INT || leftType == FLOAT) &&
                (rightType == INT || rightType == FLOAT))
            {
                return BOOL;
            }

            cout << "Semantic Error: Invalid comparison.\n";
            return ERROR;
        }

        return ERROR;
    }
};

// Assignment node: a = expression
struct Assignment : Node
{
    string name;
    Node* expression;

    Assignment(string n, Node* e)
    {
        name = n;
        expression = e;
    }

    Type checkType(map<string, Type>& table)
    {
        // Check whether variable exists
        if (table.find(name) == table.end())
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' is not declared.\n";

            return ERROR;
        }

        Type variableType = table[name];

        // Check expression
        Type expressionType =
            expression->checkType(table);

        if (expressionType == ERROR)
            return ERROR;

        // Check assignment types
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

int main()
{
    map<string, Type> symbolTable;

    int n;

    cout << "===== SEMANTIC ANALYZER =====\n\n";

    // -------------------------------
    // Variable declarations
    // -------------------------------

    cout << "Enter number of variables: ";
    cin >> n;

    cout << "\nEnter variables and their types:\n";
    cout << "Example: a int\n\n";

    for (int i = 0; i < n; i++)
    {
        string name;
        string type;

        cin >> name >> type;

        if (type == "int")
            symbolTable[name] = INT;

        else if (type == "float")
            symbolTable[name] = FLOAT;

        else if (type == "bool")
            symbolTable[name] = BOOL;

        else
        {
            cout << "Invalid type!\n";
            i--;
        }
    }

    cout << "\nDeclared variables:\n";

    for (auto x : symbolTable)
    {
        cout << x.first
             << " : "
             << typeName(x.second)
             << endl;
    }

    // -------------------------------
    // User selects an expression
    // -------------------------------

    cout << "\nChoose an example expression:\n";

    cout << "1. a = 10\n";
    cout << "2. b = a + 5\n";
    cout << "3. a = 5.5\n";
    cout << "4. c = 10\n";
    cout << "5. a = flag + 5\n";

    int choice;
    cout << "\nEnter choice: ";
    cin >> choice;

    Node* ast = NULL;

    // -------------------------------
    // Build AST
    // -------------------------------

    if (choice == 1)
    {
        ast = new Assignment(
            "a",
            new Literal(INT)
        );
    }

    else if (choice == 2)
    {
        ast = new Assignment(
            "b",
            new BinaryExpr(
                "+",
                new Variable("a"),
                new Literal(INT)
            )
        );
    }

    else if (choice == 3)
    {
        ast = new Assignment(
            "a",
            new Literal(FLOAT)
        );
    }

    else if (choice == 4)
    {
        ast = new Assignment(
            "c",
            new Literal(INT)
        );
    }

    else if (choice == 5)
    {
        ast = new Assignment(
            "a",
            new BinaryExpr(
                "+",
                new Variable("flag"),
                new Literal(INT)
            )
        );
    }

    else
    {
        cout << "Invalid choice.\n";
        return 0;
    }

    // -------------------------------
    // Semantic Analysis
    // -------------------------------

    cout << "\nPerforming semantic analysis...\n\n";

    Type result = ast->checkType(symbolTable);

    if (result != ERROR)
    {
        cout << "No semantic error found.\n";
        cout << "Expression is type correct.\n";
    }

    return 0;
}
