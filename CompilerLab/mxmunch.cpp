#include <bits/stdc++.h>
using namespace std;

const unordered_set<string> KEYWORDS = {"int", "float", "double", "char", "if", "else", "while", "for", "return", "void"};

void tokenizeMaximalMunch(const string& text) {
    int i = 0;
    int n = text.length();

    cout << left << setw(22) << "TOKEN TYPE" << setw(20) << "LEXEME" << "\n";
    cout << string(42, '-') << "\n";

    while (i < n) {
        if (isspace(text[i])) {
            i++;
            continue;
        }

        string bestLexeme = "";
        string bestTokenType = "";

        // 1. Identifiers and Keywords (Maximal Munch via alphanumeric scanning)
        if (isalpha(text[i]) || text[i] == '_') {
            int j = i;
            while (j < n && (isalnum(text[j]) || text[j] == '_')) {
                j++;
            }
            bestLexeme = text.substr(i, j - i);
            if (KEYWORDS.count(bestLexeme)) {
                bestTokenType = "KEYWORD";
            } else {
                bestTokenType = "IDENTIFIER";
            }
            i = j;
        }
        // 2. Numbers (Maximal Munch via digit/decimal scanning)
        else if (isdigit(text[i])) {
            int j = i;
            while (j < n && (isdigit(text[j]) || text[j] == '.')) {
                j++;
            }
            bestLexeme = text.substr(i, j - i);
            bestTokenType = "NUMBER";
            i = j;
        }
        // 3. Operators and Delimiters (Maximal Munch by checking longest possible matching prefix)
        else {
            static const unordered_set<string> validOps = {
                "<<=", ">>=", "==", "!=", "<=", ">=", "&&", "||", "++", "--", 
                "+=", "-=", "*=", "/=", "<<", ">>", "=", "<", ">", "+", "-", 
                "*", "/", "(", ")", "{", "}", ";", ","
            };

            // Try progressively shorter substrings starting from current index to find the longest match
            int j = n;
            string matchedOp = "";
            while (j > i) {
                string sub = text.substr(i, j - i);
                if (validOps.count(sub)) {
                    matchedOp = sub;
                    break;
                }
                j--;
            }

            if (!matchedOp.empty()) {
                bestLexeme = matchedOp;
                bestTokenType = "OPERATOR/DELIMITER";
                i += matchedOp.length();
            } else {
                bestLexeme = string(1, text[i]);
                bestTokenType = "UNKNOWN";
                i++;
            }
        }

        cout << left << setw(22) << bestTokenType << setw(20) << bestLexeme << "\n";
    }
    cout << string(42, '-') << "\n";
}

int main() {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cerr << "Error opening input.txt\n";
        return 1;
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    tokenizeMaximalMunch(content);
    return 0;
}

