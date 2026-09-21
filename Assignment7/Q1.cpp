#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

// =====================================================
// TYPES
// =====================================================

enum Type {
    INT,
    FLOAT,
    BOOL,
    ERROR,
    UNKNOWN
};

string typeName(Type t)
{
    if (t == INT) return "int";
    if (t == FLOAT) return "float";
    if (t == BOOL) return "bool";
    if (t == ERROR) return "error";
    return "unknown";
}


// =====================================================
// AST NODE
// =====================================================

struct Node
{
    virtual Type checkType(map<string, Type>& symbolTable,
                           vector<string>& errors) = 0;

    virtual ~Node() {}
};


// =====================================================
// LITERAL NODE
// =====================================================

struct Literal : Node
{
    Type type;

    Literal(Type t)
    {
        type = t;
    }

    Type checkType(map<string, Type>& symbolTable,
                   vector<string>& errors)
    {
        return type;
    }
};


// =====================================================
// VARIABLE NODE
// =====================================================

struct Variable : Node
{
    string name;

    Variable(string n)
    {
        name = n;
    }

    Type checkType(map<string, Type>& symbolTable,
                   vector<string>& errors)
    {
        if (!symbolTable.count(name))
        {
            errors.push_back(
                "Semantic Error: Variable '" +
                name +
                "' is not declared."
            );

            return ERROR;
        }

        return symbolTable[name];
    }
};


// =====================================================
// BINARY EXPRESSION NODE
// =====================================================

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

    Type checkType(map<string, Type>& symbolTable,
                   vector<string>& errors)
    {
        Type leftType =
            left->checkType(symbolTable, errors);

        Type rightType =
            right->checkType(symbolTable, errors);


        // If either side already has an error
        if (leftType == ERROR || rightType == ERROR)
            return ERROR;


        // Arithmetic operators
        if (op == "+" ||
            op == "-" ||
            op == "*" ||
            op == "/")
        {
            if ((leftType == INT || leftType == FLOAT) &&
                (rightType == INT || rightType == FLOAT))
            {
                // If either operand is float,
                // result is float.
                if (leftType == FLOAT ||
                    rightType == FLOAT)
                    return FLOAT;

                return INT;
            }

            errors.push_back(
                "Semantic Error: Invalid operands for '" +
                op +
                "'."
            );

            return ERROR;
        }


        // Relational operators
        if (op == "<" ||
            op == ">" ||
            op == "==" ||
            op == "!=")
        {
            if ((leftType == INT || leftType == FLOAT) &&
                (rightType == INT || rightType == FLOAT))
            {
                return BOOL;
            }

            errors.push_back(
                "Semantic Error: Invalid comparison using '" +
                op +
                "'."
            );

            return ERROR;
        }


        return ERROR;
    }
};


// =====================================================
// ASSIGNMENT NODE
// =====================================================

struct Assignment : Node
{
    string name;
    Node* expression;

    Assignment(string n, Node* e)
    {
        name = n;
        expression = e;
    }

    Type checkType(map<string, Type>& symbolTable,
                   vector<string>& errors)
    {
        // Check whether variable exists
        if (!symbolTable.count(name))
        {
            errors.push_back(
                "Semantic Error: Variable '" +
                name +
                "' is not declared."
            );

            return ERROR;
        }

        Type variableType =
            symbolTable[name];

        Type expressionType =
            expression->checkType(symbolTable, errors);


        if (expressionType == ERROR)
            return ERROR;


        // Exact type checking
        if (variableType != expressionType)
        {
            errors.push_back(
                "Semantic Error: Cannot assign '" +
                typeName(expressionType) +
                "' to variable '" +
                name +
                "' of type '" +
                typeName(variableType) +
                "'."
            );

            return ERROR;
        }

        return variableType;
    }
};


// =====================================================
// DECLARATION
// =====================================================

void declareVariable(
    map<string, Type>& symbolTable,
    vector<string>& errors,
    string name,
    Type type)
{
    if (symbolTable.count(name))
    {
        errors.push_back(
            "Semantic Error: Variable '" +
            name +
            "' is already declared."
        );

        return;
    }

    symbolTable[name] = type;

    cout << "Declared: "
         << name
         << " : "
         << typeName(type)
         << endl;
}


// =====================================================
// MAIN
// =====================================================

int main()
{
    map<string, Type> symbolTable;

    vector<string> errors;


    cout << "SEMANTIC ANALYZER\n";
    cout << "=================\n\n";


    // -------------------------------------------------
    // Symbol Table
    // -------------------------------------------------

    declareVariable(
        symbolTable,
        errors,
        "a",
        INT
    );

    declareVariable(
        symbolTable,
        errors,
        "b",
        FLOAT
    );

    declareVariable(
        symbolTable,
        errors,
        "flag",
        BOOL
    );


    cout << "\nPerforming semantic analysis...\n\n";


    // =================================================
    // TEST 1
    // a = 10
    // =================================================

    Node* ast1 =
        new Assignment(
            "a",
            new Literal(INT)
        );

    Type result1 =
        ast1->checkType(
            symbolTable,
            errors
        );

    if (result1 != ERROR)
        cout << "a = 10 : Type Correct\n";


    // =================================================
    // TEST 2
    // b = a + 5.5
    // =================================================

    Node* ast2 =
        new Assignment(
            "b",
            new BinaryExpr(
                "+",
                new Variable("a"),
                new Literal(FLOAT)
            )
        );

    Type result2 =
        ast2->checkType(
            symbolTable,
            errors
        );

    if (result2 != ERROR)
        cout << "b = a + 5.5 : Type Correct\n";


    // =================================================
    // TEST 3
    // a = 5.5
    // ERROR
    // =================================================

    Node* ast3 =
        new Assignment(
            "a",
            new Literal(FLOAT)
        );

    ast3->checkType(
        symbolTable,
        errors
    );


    // =================================================
    // TEST 4
    // c = 10
    // ERROR
    // =================================================

    Node* ast4 =
        new Assignment(
            "c",
            new Literal(INT)
        );

    ast4->checkType(
        symbolTable,
        errors
    );


    // =================================================
    // TEST 5
    // a = flag + 5
    // ERROR
    // =================================================

    Node* ast5 =
        new Assignment(
            "a",
            new BinaryExpr(
                "+",
                new Variable("flag"),
                new Literal(INT)
            )
        );

    ast5->checkType(
        symbolTable,
        errors
    );


    // =================================================
    // DISPLAY ERRORS
    // =================================================

    cout << "\n-----------------------------------\n";
    cout << "Semantic Analysis Result\n";
    cout << "-----------------------------------\n";

    if (errors.empty())
    {
        cout << "No semantic errors found.\n";
    }
    else
    {
        for (string error : errors)
        {
            cout << error << endl;
        }
    }


    return 0;
}
