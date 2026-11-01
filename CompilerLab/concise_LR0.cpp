#include <bits/stdc++.h>
using namespace std;

int main() {
    cout << "==================================================\n";
    cout << "           LR(0) PARSER & TABLE GENERATOR\n";
    cout << "==================================================\n\n";

    cout << "Grammar Rules:\n";
    cout << "  (0) S' -> S\n";
    cout << "  (1) S  -> A\n";
    cout << "  (2) A  -> A b\n";
    cout << "  (3) A  -> a\n\n";

    // Define Action Table: map[{State, Terminal}] = Action ("sN" for shift, "rN" for reduce, "acc")
    map<pair<int, string>, string> actionTable = {
        {{0, "a"}, "s3"},
        {{1, "$"}, "acc"},
        {{2, "b"}, "s4"}, {{2, "$"}, "r1"},
        {{3, "a"}, "r3"}, {{3, "b"}, "r3"}, {{3, "$"}, "r3"},
        {{4, "a"}, "r2"}, {{4, "b"}, "r2"}, {{4, "$"}, "r2"}
    };

    // Define Goto Table: map[{State, NonTerminal}] = NextState
    map<pair<int, string>, int> gotoTable = {
        {{0, "S"}, 1}, {{0, "A"}, 2}
    };

    // Print Parsing Table
    cout << "LR(0) Parsing Table:\n";
    cout << left << setw(8) << "State" << setw(10) << "a" << setw(10) << "b" << setw(10) << "$" << setw(10) << "S" << setw(10) << "A" << "\n";
    cout << string(58, '-') << "\n";
    for (int i = 0; i <= 4; ++i) {
        cout << left << setw(8) << i;
        vector<string> terms = {"a", "b", "$"};
        for (string t : terms) {
            string act = actionTable.count({i, t}) ? actionTable[{i, t}] : "-";
            cout << left << setw(10) << act;
        }
        vector<string> nonTerms = {"S", "A"};
        for (string nt : nonTerms) {
            string g = gotoTable.count({i, nt}) ? to_string(gotoTable[{i, nt}]) : "-";
            cout << left << setw(10) << g;
        }
        cout << "\n";
    }
    cout << string(58, '-') << "\n\n";

    // Parsing Lambda Function
    auto parseInput = [&](vector<string> tokens) {
        tokens.push_back("$");
        vector<int> stateStack = {0};
        vector<string> symbolStack = {};
        int ip = 0;

        cout << "Parsing Input: ";
        for (size_t i = 0; i < tokens.size() - 1; ++i) cout << tokens[i] << " ";
        cout << "\n";

        cout << left << setw(18) << "STATE STACK" << setw(15) << "SYMBOL" << setw(15) << "INPUT" << "ACTION\n";
        cout << string(60, '-') << "\n";

        while (true) {
            int state = stateStack.back();
            string lookahead = tokens[ip];

            string sStr = ""; for (int s : stateStack) sStr += to_string(s) + " ";
            string symStr = ""; for (const string& sym : symbolStack) symStr += sym + " ";
            string inStr = ""; for (size_t k = ip; k < tokens.size(); ++k) inStr += tokens[k] + " ";

            cout << left << setw(18) << sStr << setw(15) << symStr << setw(15) << inStr;

            if (actionTable.find({state, lookahead}) == actionTable.end()) {
                cout << "ERROR\n";
                break;
            }

            string action = actionTable[{state, lookahead}];
            if (action == "acc") {
                cout << "ACCEPT\n";
                break;
            } else if (action[0] == 's') {
                int nextState = stoi(action.substr(1));
                cout << "Shift " << nextState << "\n";
                stateStack.push_back(nextState);
                symbolStack.push_back(lookahead);
                ip++;
            } else if (action[0] == 'r') {
                int rule = stoi(action.substr(1));
                if (rule == 1) { // S -> A
                    cout << "Reduce S -> A\n";
                    stateStack.pop_back(); symbolStack.pop_back();
                    stateStack.push_back(gotoTable[{stateStack.back(), "S"}]);
                    symbolStack.push_back("S");
                } else if (rule == 2) { // A -> A b
                    cout << "Reduce A -> A b\n";
                    stateStack.pop_back(); stateStack.pop_back();
                    symbolStack.pop_back(); symbolStack.pop_back();
                    stateStack.push_back(gotoTable[{stateStack.back(), "A"}]);
                    symbolStack.push_back("A");
                } else if (rule == 3) { // A -> a
                    cout << "Reduce A -> a\n";
                    stateStack.pop_back(); symbolStack.pop_back();
                    stateStack.push_back(gotoTable[{stateStack.back(), "A"}]);
                    symbolStack.push_back("A");
                }
            }
        }
        cout << "\n";
    };

    // Test Inputs
    parseInput({"a", "b"});
    parseInput({"a", "b", "b"});

    return 0;
}