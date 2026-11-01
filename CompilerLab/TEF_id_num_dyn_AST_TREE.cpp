
#include <iostream>
#include <string>
#include <cstdlib>
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

Node* E() {
    Node* left = T();
    return Eprime(left);
}

Node* Eprime(Node* left) {
    if (input[pos] == '+' || input[pos] == '-') {
        char op = input[pos++];
        Node* right = T();

        Node* root = new Node(string(1, op));
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
    if (input[pos] == '*' || input[pos] == '/') {
        char op = input[pos++];
        Node* right = F();

        Node* root = new Node(string(1, op));
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
    if (root->value == "id") return 10;
    if (root->value == "num") return 5;

    int left = evaluate(root->left);
    int right = evaluate(root->right);

    if (root->value == "+") return left + right;
    if (root->value == "-") return left - right;
    if (root->value == "*") return left * right;
    if (root->value == "/") return left / right;

    return 0;
}

void printTree(Node* root, string prefix = "", bool isLeft = true) {
    if (root == nullptr)
        return;

    cout << prefix;
    cout << (isLeft ? "|-- " : "\\-- ");
    cout << root->value << "\n";

    if (root->left != nullptr || root->right != nullptr) {
        printTree(root->left, prefix + (isLeft ? "|   " : "    "), true);
        printTree(root->right, prefix + (isLeft ? "|   " : "    "), false);
    }
}

int main() {
    cout << "Enter expression: ";
    cin >> input;

    pos = 0;

    Node* root = E();

    if (pos != input.length())
        error();

    cout << "\nAbstract Syntax Tree:\n";
    cout << root->value << "\n";

    if (root->left != nullptr || root->right != nullptr) {
        printTree(root->left, "", true);
        printTree(root->right, "", false);
    }

    cout << "\nValue: " << evaluate(root) << endl;

    return 0;
}
