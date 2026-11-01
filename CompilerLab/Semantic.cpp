
#include <bits/stdc++.h>
using namespace std;

enum DataType { TYPE_INT, TYPE_FLOAT, TYPE_UNKNOWN, TYPE_ERROR };

string typeToString(DataType t) {
    if (t == TYPE_INT) return "int";
    if (t == TYPE_FLOAT) return "float";
    if (t == TYPE_UNKNOWN) return "unknown";
    return "error";
}

class SymbolTable {
private:
    map<string, DataType> table;

public:
    bool declare(const string& name, DataType type) {
        if (table.find(name) != table.end())
            return false;

        table[name] = type;
        return true;
    }

    DataType lookup(const string& name) {
        if (table.find(name) != table.end())
            return table[name];

        return TYPE_UNKNOWN;
    }
};

class ASTNode {
public:
    virtual DataType evaluateType(SymbolTable& st) = 0;
    virtual ~ASTNode() {}
};

class NumNode : public ASTNode {
private:
    double value;
    DataType type;

public:
    NumNode(double val, DataType t) : value(val), type(t) {}

    DataType evaluateType(SymbolTable& st) override {
        return type;
    }
};

class VarNode : public ASTNode {
private:
    string name;

public:
    VarNode(string n) : name(n) {}

    DataType evaluateType(SymbolTable& st) override {
        DataType t = st.lookup(name);

        if (t == TYPE_UNKNOWN) {
            cout << "[SEMANTIC ERROR] Variable '" << name
                 << "' used without declaration.\n";
            return TYPE_ERROR;
        }

        return t;
    }
};

class BinOpNode : public ASTNode {
private:
    ASTNode* left;
    char op;
    ASTNode* right;

public:
    BinOpNode(ASTNode* l, char o, ASTNode* r)
        : left(l), op(o), right(r) {}

    DataType evaluateType(SymbolTable& st) override {
        DataType leftType = left->evaluateType(st);
        DataType rightType = right->evaluateType(st);

        if (leftType == TYPE_ERROR || rightType == TYPE_ERROR)
            return TYPE_ERROR;

        if (leftType != rightType) {
            cout << "[SEMANTIC ERROR] Type mismatch in binary operation ('"
                 << typeToString(leftType) << "' " << op << " '"
                 << typeToString(rightType) << "').\n";
            return TYPE_ERROR;
        }

        return leftType;
    }

    ~BinOpNode() {
        delete left;
        delete right;
    }
};

class VarDeclNode : public ASTNode {
private:
    string name;
    DataType type;

public:
    VarDeclNode(string n, DataType t) : name(n), type(t) {}

    DataType evaluateType(SymbolTable& st) override {
        if (!st.declare(name, type)) {
            cout << "[SEMANTIC ERROR] Variable '" << name
                 << "' redeclared in the same scope.\n";
            return TYPE_ERROR;
        }

        return type;
    }
};

class AssignNode : public ASTNode {
private:
    string name;
    ASTNode* expr;

public:
    AssignNode(string n, ASTNode* e) : name(n), expr(e) {}

    DataType evaluateType(SymbolTable& st) override {
        DataType varType = st.lookup(name);

        if (varType == TYPE_UNKNOWN) {
            cout << "[SEMANTIC ERROR] Assignment to undeclared variable '"
                 << name << "'.\n";
            return TYPE_ERROR;
        }

        DataType exprType = expr->evaluateType(st);

        if (exprType == TYPE_ERROR)
            return TYPE_ERROR;

        if (varType != exprType) {
            cout << "[SEMANTIC ERROR] Cannot assign '"
                 << typeToString(exprType) << "' to variable '"
                 << name << "' of type '"
                 << typeToString(varType) << "'.\n";
            return TYPE_ERROR;
        }

        return varType;
    }

    ~AssignNode() {
        delete expr;
    }
};

// Tokenizes the input program
vector<string> tokenize(string s) {
    vector<string> tokens;
    int i = 0;

    while (i < (int)s.size()) {
        if (isspace(s[i])) {
            i++;
            continue;
        }

        if (isalpha(s[i]) || s[i] == '_') {
            string word = "";

            while (i < (int)s.size() &&
                   (isalnum(s[i]) || s[i] == '_')) {
                word += s[i++];
            }

            tokens.push_back(word);
        }
        else if (isdigit(s[i])) {
            string num = "";

            while (i < (int)s.size() &&
                   isdigit(s[i])) {
                num += s[i++];
            }

            if (i < (int)s.size() && s[i] == '.') {
                num += s[i++];

                while (i < (int)s.size() &&
                       isdigit(s[i])) {
                    num += s[i++];
                }
            }

            tokens.push_back(num);
        }
        else {
            tokens.push_back(string(1, s[i]));
            i++;
        }
    }

    return tokens;
}

class Parser {
private:
    vector<string> tokens;
    int pos = 0;

    string peek() {
        if (pos >= (int)tokens.size())
            return "";

        return tokens[pos];
    }

    string get() {
        if (pos >= (int)tokens.size())
            return "";

        return tokens[pos++];
    }

    void match(string s) {
        if (get() != s) {
            cout << "Syntax Error: Expected " << s << "\n";
            exit(0);
        }
    }

    ASTNode* E() {
        ASTNode* left = T();

        while (peek() == "+" || peek() == "-") {
            char op = get()[0];
            ASTNode* right = T();
            left = new BinOpNode(left, op, right);
        }

        return left;
    }

    ASTNode* T() {
        ASTNode* left = F();

        while (peek() == "*" || peek() == "/") {
            char op = get()[0];
            ASTNode* right = F();
            left = new BinOpNode(left, op, right);
        }

        return left;
    }

    ASTNode* F() {
        string s = peek();

        if (s == "(") {
            get();
            ASTNode* root = E();
            match(")");
            return root;
        }

        if (s == "") {
            cout << "Syntax Error\n";
            exit(0);
        }

        if (isdigit(s[0])) {
            get();

            if (s.find('.') != string::npos)
                return new NumNode(stod(s), TYPE_FLOAT);

            return new NumNode(stod(s), TYPE_INT);
        }

        if (isalpha(s[0]) || s[0] == '_') {
            get();
            return new VarNode(s);
        }

        cout << "Syntax Error near " << s << "\n";
        exit(0);

        return nullptr;
    }

public:
    Parser(vector<string> t) {
        tokens = t;
    }

    vector<ASTNode*> parseProgram() {
        vector<ASTNode*> program;

        while (pos < (int)tokens.size()) {
            string s = peek();

            if (s == "int" || s == "float") {
                get();

                DataType type = (s == "int") ? TYPE_INT : TYPE_FLOAT;
                string name = get();

                program.push_back(new VarDeclNode(name, type));

                if (peek() == "=") {
                    get();
                    ASTNode* expr = E();
                    program.push_back(new AssignNode(name, expr));
                }
            }
            else if (pos + 1 < (int)tokens.size() &&
                    tokens[pos + 1] == "=") {
                string name = get();
                get();

                ASTNode* expr = E();
                program.push_back(new AssignNode(name, expr));
            }
            else if (s == "cout") {
                get();
                match("<");
                match("<");

                program.push_back(E());
            }
            else {
                program.push_back(E());
            }

            match(";");
        }

        return program;
    }
};

int main() {
    cout << "============================================\n";
    cout << "       AST SEMANTIC ANALYZER\n";
    cout << "============================================\n";

    cout << "\nEnter your program (type END to finish):\n";

    string line, source = "";

    while (true) {
        getline(cin, line);

        if (line == "END")
            break;

        source += line + " ";
    }

    vector<string> tokens = tokenize(source);

    Parser parser(tokens);
    vector<ASTNode*> program = parser.parseProgram();

    SymbolTable symbolTable;

    cout << "\nSemantic Analysis:\n";

    for (int i = 0; i < (int)program.size(); i++) {
        cout << "\nStatement " << i + 1 << ": ";

        DataType dt = program[i]->evaluateType(symbolTable);

        if (dt != TYPE_ERROR)
            cout << "Successfully checked. Type: "
                 << typeToString(dt) << "\n";
        else
            cout << "Semantic analysis failed.\n";

        delete program[i];
    }

    cout << "\n============================================\n";

    return 0;
}
