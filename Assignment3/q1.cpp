#include <iostream>
#include <string>
#include <cctype>
using namespace std;

struct Node {
    string value;
    Node *left, *right;

    Node(string v) {
        value = v;
        left = right = nullptr;
    }
};

string input;
int pos = 0;

void error() {
    cout << "Invalid expression\n";
    exit(0);
}

Node* E();
Node* Eprime(Node*);
Node* T();
Node* Tprime(Node*);
Node* F();

void match(string token) {
    if (input.substr(pos, token.length()) == token)
        pos += token.length();
    else
        error();
}

Node* E() {
    Node* left = T();
    return Eprime(left);
}

Node* Eprime(Node* left) {
    if (input[pos] == '+') {
        pos++;
        Node* right = T();
        Node* root = new Node("+");
        root->left = left;
        root->right = right;
        return Eprime(root);
    }

    if (input[pos] == '-') {
        pos++;
        Node* right = T();
        Node* root = new Node("-");
        root->left = left;
        root->right = right;
        return Eprime(root);
    }

    return left;
}

Node* T() {
    Node* left = F();
    return Tprime(left);
}

Node* Tprime(Node* left) {
    if (input[pos] == '*') {
        pos++;
        Node* right = F();
        Node* root = new Node("*");
        root->left = left;
        root->right = right;
        return Tprime(root);
    }

    if (input[pos] == '/') {
        pos++;
        Node* right = F();
        Node* root = new Node("/");
        root->left = left;
        root->right = right;
        return Tprime(root);
    }

    return left;
}

Node* F() {
    if (input[pos] == '(') {
        pos++;
        Node* root = E();

        if (input[pos] != ')')
            error();

        pos++;
        return root;
    }

    if (input.substr(pos, 2) == "id") {
        pos += 2;
        return new Node("id");
    }

    if (input.substr(pos, 3) == "num") {
        pos += 3;
        return new Node("num");
    }

    error();
    return nullptr;
}

int evaluate(Node* root) {
    if (root->value == "id")
        return 10;

    if (root->value == "num")
        return 5;

    int left = evaluate(root->left);
    int right = evaluate(root->right);

    if (root->value == "+")
        return left + right;

    if (root->value == "-")
        return left - right;

    if (root->value == "*")
        return left * right;

    if (root->value == "/")
        return left / right;

    return 0;
}

void printAST(Node* root, int level = 0) {
    if (root == nullptr)
        return;

    for (int i = 0; i < level; i++)
        cout << "  ";

    cout << root->value << endl;

    printAST(root->left, level + 1);
    printAST(root->right, level + 1);
}

int main() {
    string expressions[] = {
        "id+id*num",
        "(id-num)/id"
    };

    for (string exp : expressions) {
        input = exp;
        pos = 0;

        cout << "\nExpression: " << input << endl;

        Node* root = E();

        if (pos != input.length()) {
            error();
        }

        cout << "\nAST:\n";
        printAST(root);

        cout << "\nValue: " << evaluate(root) << endl;
    }

    return 0;
}
