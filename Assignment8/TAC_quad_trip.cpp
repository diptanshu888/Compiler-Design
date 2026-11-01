#include <bits/stdc++.h>
using namespace std;

struct Token {
    string type;
    string value;
};

struct TAC {
    string op, a1, a2, res;
};

vector<Token> tokens;
vector<TAC> code;
int pos = 0;
int tempNo = 1;

bool isId(const string &s) {
    if (s.empty() || (!isalpha(s[0]) && s[0] != '_'))
        return false;

    for (int i = 1; i < (int)s.size(); i++) {
        if (!isalnum(s[i]) && s[i] != '_')
            return false;
    }
    return true;
}

void lexical(string s) {
    int i = 0;

    while (i < (int)s.size()) {
        if (isspace(s[i])) {
            i++;
            continue;
        }

        if (isalpha(s[i]) || s[i] == '_') {
            string x;

            while (i < (int)s.size() &&
                   (isalnum(s[i]) || s[i] == '_')) {
                x += s[i++];
            }

            tokens.push_back({"id", x});
        }
        else if (isdigit(s[i])) {
            string x;

            while (i < (int)s.size() && isdigit(s[i])) {
                x += s[i++];
            }

            tokens.push_back({"num", x});
        }
        else if (s[i] == '+' || s[i] == '-' ||
                 s[i] == '*' || s[i] == '/' ||
                 s[i] == '=' || s[i] == '(' ||
                 s[i] == ')') {
            string x(1, s[i]);

            if (s[i] == '+' || s[i] == '-' ||
                s[i] == '*' || s[i] == '/') {
                tokens.push_back({"op", x});
            }
            else if (s[i] == '=') {
                tokens.push_back({"assign", x});
            }
            else {
                tokens.push_back({"paren", x});
            }

            i++;
        }
        else {
            cout << "Invalid character: " << s[i] << endl;
            exit(0);
        }
    }

    tokens.push_back({"end", "$"});
}

string newTemp() {
    return "t" + to_string(tempNo++);
}

void emit(string op, string a1, string a2, string res) {
    code.push_back({op, a1, a2, res});
}

void error() {
    cout << "Syntax Error\n";
    exit(0);
}

string expr();

string factor() {
    if (tokens[pos].type == "id" ||
        tokens[pos].type == "num") {
        return tokens[pos++].value;
    }

    if (tokens[pos].value == "(") {
        pos++;
        string x = expr();

        if (tokens[pos].value != ")")
            error();

        pos++;
        return x;
    }

    error();
    return "";
}

string term() {
    string x = factor();

    while (tokens[pos].value == "*" ||
           tokens[pos].value == "/") {

        string op = tokens[pos++].value;
        string y = factor();
        string t = newTemp();

        emit(op, x, y, t);
        x = t;
    }

    return x;
}

string expr() {
    string x = term();

    while (tokens[pos].value == "+" ||
           tokens[pos].value == "-") {

        string op = tokens[pos++].value;
        string y = term();
        string t = newTemp();

        emit(op, x, y, t);
        x = t;
    }

    return x;
}

void parse() {
    if (tokens[0].type == "id" &&
        tokens[1].type == "assign") {

        string lhs = tokens[pos++].value;
        pos++;

        string x = expr();

        if (tokens[pos].type != "end")
            error();

        emit("=", x, "", lhs);
    }
    else {
        string x = expr();

        if (tokens[pos].type != "end")
            error();

        cout << "\nFinal Result: " << x << endl;
    }
}

void printTokens() {
    cout << "\nTokens:\n";

    for (int i = 0; i < (int)tokens.size() - 1; i++) {
        cout << "(" << tokens[i].type
             << ", " << tokens[i].value << ")\n";
    }
}

void printTAC() {
    cout << "\nThree Address Code:\n";

    for (auto x : code) {
        if (x.op == "=")
            cout << x.res << " = " << x.a1 << endl;
        else
            cout << x.res << " = "
                 << x.a1 << " "
                 << x.op << " "
                 << x.a2 << endl;
    }
}

void printQuadruple() {
    cout << "\nQuadruple:\n";
    cout << "Op\tArg1\tArg2\tResult\n";

    for (auto x : code) {
        cout << x.op << "\t"
             << x.a1 << "\t"
             << (x.a2.empty() ? "-" : x.a2) << "\t"
             << x.res << endl;
    }
}

string tripleArg(string s) {
    if (s.size() > 1 && s[0] == 't') {
        bool ok = true;

        for (int i = 1; i < (int)s.size(); i++) {
            if (!isdigit(s[i]))
                ok = false;
        }

        if (ok) {
            int n = stoi(s.substr(1)) - 1;
            return "(" + to_string(n) + ")";
        }
    }

    return s;
}

void printTriple() {
    cout << "\nTriple:\n";
    cout << "Index\tOp\tArg1\tArg2\n";

    for (int i = 0; i < (int)code.size(); i++) {
        string a1 = tripleArg(code[i].a1);
        string a2 = code[i].a2.empty() ? "-" : tripleArg(code[i].a2);

        cout << i << "\t"
             << code[i].op << "\t"
             << a1 << "\t"
             << a2 << endl;
    }
}

int main() {
    string s;

    cout << "Enter expression: ";
    getline(cin, s);

    lexical(s);
    printTokens();
    parse();
    printTAC();
    printQuadruple();
    printTriple();

    return 0;
}
