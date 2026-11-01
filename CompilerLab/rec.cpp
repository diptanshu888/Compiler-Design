#include <bits/stdc++.h>
using namespace std;

enum TokenType { TOKEN_ID, TOKEN_NUM, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_END };

struct Token {
    TokenType type;
    string val;
};

class Lexer {
    string src;
    size_t pos;
public:
    Lexer(const string& s) : src(s), pos(0) {}

    Token nextToken() {
        while (pos < src.length() && isspace(src[pos])) pos++;
        if (pos >= src.length()) return {TOKEN_END, ""};

        char c = src[pos];
        if (isalpha(c) || c == '_') {
            string s = "";
            while (pos < src.length() && (isalnum(src[pos]) || src[pos] == '_')) {
                s += src[pos++];
            }
            return {TOKEN_ID, s};
        }
        if (isdigit(c)) {
            string s = "";
            while (pos < src.length() && (isdigit(src[pos]) || src[pos] == '.')) {
                s += src[pos++];
            }
            return {TOKEN_NUM, s};
        }
        if (c == '+') { pos++; return {TOKEN_PLUS, "+"}; }
        if (c == '-') { pos++; return {TOKEN_MINUS, "-"}; }
        if (c == '*') { pos++; return {TOKEN_MUL, "*"}; }
        if (c == '/') { pos++; return {TOKEN_DIV, "/"}; }
        if (c == '(') { pos++; return {TOKEN_LPAREN, "("}; }
        if (c == ')') { pos++; return {TOKEN_RPAREN, ")"}; }
        pos++;
        return {TOKEN_END, ""};
    }
};

struct ASTNode {
    string val;
    ASTNode *left, *right;
    ASTNode(string v, ASTNode* l = nullptr, ASTNode* r = nullptr) : val(v), left(l), right(r) {}
};

class Parser {
    Lexer lexer;
    Token currentToken;

    void advance() {
        currentToken = lexer.nextToken();
    }

    ASTNode* parseE() {
        ASTNode* tNode = parseT();
        return parseEPrime(tNode);
    }

    ASTNode* parseEPrime(ASTNode* left) {
        if (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS) {
            string op = currentToken.val;
            advance();
            ASTNode* tNode = parseT();
            ASTNode* newNode = new ASTNode(op, left, tNode);
            return parseEPrime(newNode);
        }
        return left;
    }

    ASTNode* parseT() {
        ASTNode* fNode = parseF();
        return parseTPrime(fNode);
    }

    ASTNode* parseTPrime(ASTNode* left) {
        if (currentToken.type == TOKEN_MUL || currentToken.type == TOKEN_DIV) {
            string op = currentToken.val;
            advance();
            ASTNode* fNode = parseF();
            ASTNode* newNode = new ASTNode(op, left, fNode);
            return parseTPrime(newNode);
        }
        return left;
    }

    ASTNode* parseF() {
        if (currentToken.type == TOKEN_ID || currentToken.type == TOKEN_NUM) {
            ASTNode* node = new ASTNode(currentToken.val);
            advance();
            return node;
        }
        if (currentToken.type == TOKEN_LPAREN) {
            advance();
            ASTNode* node = parseE();
            if (currentToken.type == TOKEN_RPAREN) {
                advance();
            }
            return node;
        }
        return nullptr;
    }

public:
    Parser(const string& s) : lexer(s) {
        advance();
    }

    ASTNode* parse() {
        return parseE();
    }
};

void printAST(ASTNode* node, string indent = "", bool last = true) {
    if (!node) return;
    cout << indent;
    if (last) {
        cout << "R----";
        indent += "     ";
    } else {
        cout << "L----";
        indent += "|    ";
    }
    cout << node->val << "\n";
    printAST(node->left, indent, false);
    printAST(node->right, indent, true);
}

double evaluate(ASTNode* node, map<string, double>& symbolTable) {
    if (!node) return 0;
    if (node->val == "+") return evaluate(node->left, symbolTable) + evaluate(node->right, symbolTable);
    if (node->val == "-") return evaluate(node->left, symbolTable) - evaluate(node->right, symbolTable);
    if (node->val == "*") return evaluate(node->left, symbolTable) * evaluate(node->right, symbolTable);
    if (node->val == "/") return evaluate(node->left, symbolTable) / evaluate(node->right, symbolTable);
    
    if (isdigit(node->val[0])) {
        return stod(node->val);
    }
    return symbolTable[node->val];
}

void processExpression(string expr, map<string, double>& symbolTable) {
    cout << "\n========================================\n";
    cout << "Expression: " << expr << "\n";
    cout << "========================================\n";
    
    Parser parser(expr);
    ASTNode* root = parser.parse();
    
    cout << "Abstract Syntax Tree (AST):\n";
    printAST(root);
    
    double result = evaluate(root, symbolTable);
    cout << "Evaluated Result: " << result << "\n";
}

int main() {
    map<string, double> symbolTable = {
        {"id", 10.0},
        {"num", 4.0}
    };

    cout << "Symbol Table Values: id = 10.0, num = 4.0\n";

    processExpression("id+id*num", symbolTable);
    processExpression("(id-num)/id", symbolTable);

    return 0;
}