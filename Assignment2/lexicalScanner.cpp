#include <bits/stdc++.h>
using namespace std;

const unordered_set<string> KEYWORDS = {
    "int", "float", "double", "char", "if", "else", "while", "for", "return", "void"
};

bool isKeyword(const string& s) {
    return KEYWORDS.find(s) != KEYWORDS.end();
}

void tokenize(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << "\n";
        return;
    }

    string line;
    int lineNum = 0;

    cout << "\n" << left << setw(22) << "TOKEN TYPE" << setw(20) << "LEXEME" << "LINE" << "\n";
    cout << string(52, '-') << "\n";

    while (getline(file, line)) {
        lineNum++;
        size_t i = 0;
        while (i < line.length()) {
            char c = line[i];

            if (isspace(c)) {
                i++;
                continue;
            }

            if (string("+-*/=<>(){}[];,").find(c) != string::npos) {
                string op(1, c);
                if (i + 1 < line.length()) {
                    string doubleOp = op + line[i + 1];
                    if (doubleOp == "==" || doubleOp == "<=" || doubleOp == ">=" || doubleOp == "!=") {
                        op = doubleOp;
                        i++;
                    }
                }
                cout << left << setw(22) << "OPERATOR/DELIMITER" << setw(20) << op << lineNum << "\n";
                i++;
                continue;
            }

            if (isalpha(c) || c == '_') {
                string lexeme = "";
                while (i < line.length() && (isalnum(line[i]) || line[i] == '_')) {
                    lexeme += line[i];
                    i++;
                }
                if (isKeyword(lexeme)) {
                    cout << left << setw(22) << "KEYWORD" << setw(20) << lexeme << lineNum << "\n";
                } else {
                    cout << left << setw(22) << "IDENTIFIER" << setw(20) << lexeme << lineNum << "\n";
                }
                continue;
            }

            if (isdigit(c)) {
                string lexeme = "";
                while (i < line.length() && (isdigit(line[i]) || line[i] == '.')) {
                    lexeme += line[i];
                    i++;
                }
                cout << left << setw(22) << "NUMBER" << setw(20) << lexeme << lineNum << "\n";
                continue;
            }

            i++;
        }
    }
    file.close();
    cout << string(52, '-') << "\n";
}

int main() {
    tokenize("input.txt");
    return 0;
}
