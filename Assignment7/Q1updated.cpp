#include <iostream>
#include <string>
#include <map>
using namespace std;

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

// Base AST node
struct Node
{
    virtual Type checkType(map<string, Type>& table) = 0;
    virtual ~Node() {}
};

// Literal node: 10, 5.5, true
struct Literal : Node
{
    Type type;

    Literal(Type t)
    {
        type = t;
    }

    Type checkType(map<string, Type>& table)
    {
        return type;
    }
};

// Variable node: a, b, flag
struct Variable : Node
{
    string name;

    Variable(string n)
    {
        name = n;
    }

    Type checkType(map<string, Type>& table)
    {
        if (table.find(name) == table.end())
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' is not declared.\n";

            return ERROR;
        }

        return table[name];
    }
};

// Binary expression node: a + 5
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

    Type checkType(map<string, Type>& table)
    {
        Type leftType = left->checkType(table);
        Type rightType = right->checkType(table);

        if (leftType == ERROR || rightType == ERROR)
            return ERROR;

        // Arithmetic operators
        if (op == "+" || op == "-" ||
            op == "*" || op == "/")
        {
            if ((leftType == INT || leftType == FLOAT) &&
                (rightType == INT || rightType == FLOAT))
            {
                // int + float -> float
                if (leftType == FLOAT ||
                    rightType == FLOAT)
                    return FLOAT;

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
