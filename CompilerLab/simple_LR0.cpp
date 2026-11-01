#include <bits/stdc++.h>
using namespace std;

// Grammar:
// 0: S' -> S
// 1: S  -> A
// 2: A  -> A b
// 3: A  -> a

struct Production {
    int lhs; // Non-terminal index
    vector<string> rhs;
};

void printLR0ItemsAndTable() {
    cout << "========================================\n";
    cout << "        LR(0) PARSER & TABLE GENERATOR\n";
    cout << "========================================\n\n";

    cout << "Grammar Rules:\n";
    cout << "  (0) S' -> S\n";
    cout << "  (1) S  -> A\n";
    cout << "  (2) A  -> A b\n";
    cout << "  (3) A  -> a\n\n";

    cout << "Canonical Collection of LR(0) States:\n";
    cout << "----------------------------------------\n";
    cout << "State 0:\n";
    cout << "  S' -> . S\n";
    cout << "  S  -> . A\n";
    cout << "  A  -> . A b\n";
    cout << "  A  -> . a\n\n";

    cout << "State 1:\n";
    cout << "  S' -> S .\n\n";

    cout << "State 2:\n";
    cout << "  S  -> A .\n";
    cout << "  A  -> A . b\n\n";

    cout << "State 3:\n";
    cout << "  A  -> a .\n\n";

    cout << "State 4:\n";
    cout << "  A  -> A b .\n";
    cout << "----------------------------------------\n\n";

    cout << "LR(0) Parsing Table (ACTION & GOTO):\n";
    cout << left << setw(8) << "State" << setw(10) << "a" << setw(10) << "b" << setw(10) << "$" << setw(10) << "S" << setw(10) << "A" << "\n";
    cout << string(58, '-') << "\n";
    cout << left << setw(8) << "0" << setw(10) << "s3" << setw(10) << "-" << setw(10) << "-" << setw(10) << "1" << setw(10) << "2" << "\n";
    cout << left << setw(8) << "1" << setw(10) << "-" << setw(10) << "-" << setw(10) << "acc" << setw(10) << "-" << setw(10) << "-" << "\n";
    cout << left << setw(8) << "2" << setw(10) << "-" << setw(10) << "s4" << setw(10) << "r1" << setw(10) << "-" << setw(10) << "-" << "\n";
    cout << left << setw(8) << "3" << setw(10) << "r3" << setw(10) << "r3" << setw(10) << "r3" << setw(10) << "-" << setw(10) << "-" << "\n";
    cout << left << setw(8) << "4" << setw(10) << "r2" << setw(10) << "r2" << setw(10) << "r2" << setw(10) << "-" << setw(10) << "-" << "\n";
    cout << string(58, '-') << "\n\n";
}

void parseInput(string input) {
    input += "$";
    vector<int> stateStack = {0};
    vector<char> symbolStack = {};
    int ip = 0;

    cout << "Parsing Input: " << input << "\n";
    cout << left << setw(20) << "STACK (States)" << setw(20) << "SYMBOL STACK" << setw(15) << "INPUT" << "ACTION\n";
    cout << string(70, '-') << "\n";

    while (true) {
        int state = stateStack.back();
        char lookahead = input[ip];

        string stateStr = "";
        for (int s : stateStack) stateStr += to_string(s) + " ";
        string symStr = "";
        for (char c : symbolStack) symStr += c;
        string inStr = input.substr(ip);

        cout << left << setw(20) << stateStr << setw(20) << symStr << setw(15) << inStr;

        // ACTION TABLE LOOKUP
        if (state == 0) {
            if (lookahead == 'a') {
                cout << "Shift 3\n";
                stateStack.push_back(3);
                symbolStack.push_back('a');
                ip++;
            } else {
                cout << "Error\n";
                break;
            }
        } else if (state == 1) {
            if (lookahead == '$') {
                cout << "Accept\n";
                break;
            } else {
                cout << "Error\n";
                break;
            }
        } else if (state == 2) {
            if (lookahead == 'b') {
                cout << "Shift 4\n";
                stateStack.push_back(4);
                symbolStack.push_back('b');
                ip++;
            } else if (lookahead == '$') {
                cout << "Reduce by S -> A\n";
                stateStack.pop_back();
                symbolStack.pop_back();
                int topState = stateStack.back();
                if (topState == 0) {
                    stateStack.push_back(1);
                    symbolStack.push_back('S');
                }
            } else {
                cout << "Error\n";
                break;
            }
        } else if (state == 3) {
            cout << "Reduce by A -> a\n";
            stateStack.pop_back();
            symbolStack.pop_back();
            int topState = stateStack.back();
            if (topState == 0) {
                stateStack.push_back(2);
                symbolStack.push_back('A');
            }
        } else if (state == 4) {
            cout << "Reduce by A -> Ab\n";
            stateStack.pop_back();
            stateStack.pop_back();
            symbolStack.pop_back();
            symbolStack.pop_back();
            int topState = stateStack.back();
            if (topState == 0) {
                stateStack.push_back(2);
                symbolStack.push_back('A');
            }
        } else {
            cout << "Error\n";
            break;
        }
    }
    cout << "\n";
}

int main() {
    printLR0ItemsAndTable();

    parseInput("ab");
    parseInput("abb");

    return 0;
}