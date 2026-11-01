#include <bits/stdc++.h>
using namespace std;

// Structure for Three Address Code (TAC)
struct TAC {
    string op;
    string arg1;
    string arg2;
    string result;
};

// Global variables for parser and code generation
vector<string> tokens;
int tokenIndex = 0;
int tempCount = 1;
vector<TAC> tacList;

// Helper to generate new temporary variables
string newTemp() {
    return "t" + to_string(tempCount++);
}

// Simple Lexer: Tokenizes string like "a = b + c * d"
void tokenize(const string& str) {
    tokens.clear();
    string curr = "";
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (isspace(c)) continue;
        if (isalnum(c)) {
            curr += c;
        } else {
            if (!curr.empty()) {
                tokens.push_back(curr);
                curr = "";
            }
            tokens.push_back(string(1, c));
        }
    }
    if (!curr.empty()) {
        tokens.push_back(curr);
    }
}

// Forward declarations for Recursive Descent Parser
string parseExpr();
string parseTerm();
string parseFactor();

// Factor -> id | num | ( Expr )
string parseFactor() {
    if (tokenIndex >= tokens.size()) return "";
    string token = tokens[tokenIndex++];
    if (token == "(") {
        string val = parseExpr();
        tokenIndex++; // consume ')'
        return val;
    }
    return token; // identifier or number
}

// Term -> Factor (( "*" | "/" ) Factor)*
string parseTerm() {
    string left = parseFactor();
    while (tokenIndex < tokens.size() && (tokens[tokenIndex] == "*" || tokens[tokenIndex] == "/")) {
        string op = tokens[tokenIndex++];
        string right = parseFactor();
        string temp = newTemp();
        tacList.push_back({op, left, right, temp});
        left = temp;
    }
    return left;
}

// Expr -> Term (( "+" | "-" ) Term)*
string parseExpr() {
    string left = parseTerm();
    while (tokenIndex < tokens.size() && (tokens[tokenIndex] == "+" || tokens[tokenIndex] == "-")) {
        string op = tokens[tokenIndex++];
        string right = parseTerm();
        string temp = newTemp();
        tacList.push_back({op, left, right, temp});
        left = temp;
    }
    return left;
}

// Assignment -> id = Expr
void parseAssignment() {
    if (tokenIndex + 1 < tokens.size() && tokens[tokenIndex + 1] == "=") {
        string lhs = tokens[tokenIndex];
        tokenIndex += 2; // skip id and '='
        string rhsVal = parseExpr();
        tacList.push_back({"=", rhsVal, "", lhs});
    } else {
        parseExpr();
    }
}

int main() {
    string inputExpr;
    cout << "==================================================\n";
    cout << "     TAC, QUADRUPLE & TRIPLE GENERATOR           \n";
    cout << "==================================================\n";
    cout << "Enter an assignment expression (e.g., a = b + c * d):\n> ";
    getline(cin, inputExpr);

    tokenize(inputExpr);
    tokenIndex = 0;
    tacList.clear();
    tempCount = 1;

    parseAssignment();

    // 1. Display Three Address Code (TAC)
    cout << "\n--------------------------------------------------\n";
    cout << "1. THREE ADDRESS CODE (TAC)\n";
    cout << "--------------------------------------------------\n";
    for (size_t i = 0; i < tacList.size(); ++i) {
        if (tacList[i].op == "=") {
            cout << tacList[i].result << " = " << tacList[i].arg1 << "\n";
        } else {
            cout << tacList[i].result << " = " << tacList[i].arg1 << " " << tacList[i].op << " " << tacList[i].arg2 << "\n";
        }
    }

    // 2. Display Quadruple Representation
    cout << "\n--------------------------------------------------\n";
    cout << "2. QUADRUPLE REPRESENTATION\n";
    cout << "--------------------------------------------------\n";
    cout << left << setw(8) << "Index" << setw(10) << "Op" << setw(12) << "Arg1" << setw(12) << "Arg2" << setw(10) << "Result" << "\n";
    cout << string(52, '-') << "\n";
    for (size_t i = 0; i < tacList.size(); ++i) {
        cout << left << setw(8) << i 
             << setw(10) << tacList[i].op 
             << setw(12) << tacList[i].arg1 
             << setw(12) << (tacList[i].arg2.empty() ? "-" : tacList[i].arg2) 
             << setw(10) << tacList[i].result << "\n";
    }

    // 3. Display Triple Representation
    cout << "\n--------------------------------------------------\n";
    cout << "3. TRIPLE REPRESENTATION\n";
    cout << "--------------------------------------------------\n";
    cout << left << setw(8) << "Index" << setw(10) << "Op" << setw(12) << "Arg1" << setw(12) << "Arg2" << "\n";
    cout << string(42, '-') << "\n";

    // For triples, we map temporary variables to their instruction index (e.g., t1 -> (0))
    vector<string> tempMap(tempCount, "");
    for (size_t i = 0; i < tacList.size(); ++i) {
        string arg1 = tacList[i].arg1;
        string arg2 = tacList[i].arg2;

        // Replace temp references with parenthesized indices
        for (int t = 1; t < tempCount; ++t) {
            string tempName = "t" + to_string(t);
            if (arg1 == tempName) arg1 = "(" + tempMap[t] + ")";
            if (arg2 == tempName) arg2 = "(" + tempMap[t] + ")";
        }

        cout << left << setw(8) << i 
             << setw(10) << tacList[i].op 
             << setw(12) << arg1 
             << setw(12) << (arg2.empty() ? "-" : arg2) << "\n";

        if (tacList[i].result.rfind("t", 0) == 0) {
            int tNum = stoi(tacList[i].result.substr(1));
            tempMap[tNum] = to_string(i);
        }
    }
    cout << string(42, '-') << "\n\n";

    return 0;
}