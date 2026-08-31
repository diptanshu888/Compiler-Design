#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <iomanip>
#include <algorithm>

using namespace std;

int main() {

    /*
        Grammar:

        0: S' -> S
        1: S  -> A
        2: A  -> Ab
        3: A  -> a
    */

    vector<pair<char, string>> prod = {
        {'Z', "S"},
        {'S', "A"},
        {'A', "Ab"},
        {'A', "a"}
    };

    set<char> nonTerminals = {'Z', 'S', 'A'};
    set<char> terminals = {'a', 'b'};

    // ------------------------------------------------
    // CLOSURE FUNCTION
    // Item = {production number, dot position}
    // ------------------------------------------------

    auto closure = [&](set<pair<int, int>> items) {

        bool changed = true;

        while (changed) {

            changed = false;

            vector<pair<int, int>> add;

            for (auto item : items) {

                int p = item.first;
                int dot = item.second;

                string rhs = prod[p].second;

                // Symbol after dot
                if (dot < rhs.size()) {

                    char symbol = rhs[dot];

                    // If it is a non-terminal
                    if (nonTerminals.count(symbol)) {

                        // Add all productions of that symbol
                        for (int i = 0; i < prod.size(); i++) {

                            if (prod[i].first == symbol) {

                                pair<int, int> newItem = {i, 0};

                                if (!items.count(newItem))
                                    add.push_back(newItem);
                            }
                        }
                    }
                }
            }

            for (auto x : add) {

                if (!items.count(x)) {
                    items.insert(x);
                    changed = true;
                }
            }
        }

        return items;
    };


    // ------------------------------------------------
    // GOTO FUNCTION
    // ------------------------------------------------

    auto goTo = [&](set<pair<int, int>> items, char symbol) {

        set<pair<int, int>> next;

        for (auto item : items) {

            int p = item.first;
            int dot = item.second;

            string rhs = prod[p].second;

            if (dot < rhs.size() && rhs[dot] == symbol) {

                next.insert({p, dot + 1});
            }
        }

        return closure(next);
    };


    // ------------------------------------------------
    // STEP 1: CONSTRUCT LR(0) STATES
    // ------------------------------------------------

    vector<set<pair<int, int>>> states;

    map<int, map<char, int>> transitions;

    set<pair<int, int>> start;

    // Z -> .S
    start.insert({0, 0});

    states.push_back(closure(start));

    queue<int> q;

    q.push(0);

    set<char> symbols = {'a', 'b', 'S', 'A'};

    while (!q.empty()) {

        int current = q.front();
        q.pop();

        for (char symbol : symbols) {

            set<pair<int, int>> next =
                goTo(states[current], symbol);

            if (next.empty())
                continue;

            int nextState = -1;

            // Check whether state already exists
            for (int i = 0; i < states.size(); i++) {

                if (states[i] == next) {
                    nextState = i;
                    break;
                }
            }

            // New state
            if (nextState == -1) {

                nextState = states.size();

                states.push_back(next);

                q.push(nextState);
            }

            transitions[current][symbol] = nextState;
        }
    }


    // ------------------------------------------------
    // PRINT LR(0) ITEM SETS
    // ------------------------------------------------

    cout << "\nCANONICAL LR(0) ITEMS\n";
    cout << "==========================\n";

    for (int i = 0; i < states.size(); i++) {

        cout << "\nI" << i << ":\n";

        for (auto item : states[i]) {

            int p = item.first;
            int dot = item.second;

            cout << prod[p].first << " -> ";

            string rhs = prod[p].second;

            for (int j = 0; j <= rhs.size(); j++) {

                if (j == dot)
                    cout << ".";

                if (j < rhs.size())
                    cout << rhs[j];
            }

            cout << "\n";
        }
    }


    // ------------------------------------------------
    // STEP 2: CONSTRUCT LR(0) TABLE
    // ------------------------------------------------

    vector<char> terminalList = {'a', 'b', '$'};
    vector<char> nonTerminalList = {'S', 'A'};

    map<int, map<char, string>> ACTION;
    map<int, map<char, int>> GOTO;

    bool conflict = false;


    // Add action and detect conflict
    auto addAction = [&](int state, char symbol, string value) {

        if (ACTION[state].count(symbol) &&
            ACTION[state][symbol] != value) {

            cout << "\nCONFLICT at ACTION["
                 << state << "," << symbol << "] : "
                 << ACTION[state][symbol]
                 << " / "
                 << value << "\n";

            ACTION[state][symbol] += "/" + value;

            conflict = true;
        }
        else {

            ACTION[state][symbol] = value;
        }
    };


    for (int i = 0; i < states.size(); i++) {

        // SHIFT ACTIONS
        for (char t : terminals) {

            if (transitions[i].count(t)) {

                addAction(
                    i,
                    t,
                    "s" + to_string(transitions[i][t])
                );
            }
        }


        // GOTO TABLE
        for (char nt : nonTerminalList) {

            if (transitions[i].count(nt)) {

                GOTO[i][nt] = transitions[i][nt];
            }
        }


        // REDUCE / ACCEPT
        for (auto item : states[i]) {

            int p = item.first;
            int dot = item.second;

            string rhs = prod[p].second;

            // Completed item
            if (dot == rhs.size()) {

                // Z -> S.
                if (p == 0) {

                    addAction(i, '$', "acc");
                }

                else {

                    // LR(0): reduce on ALL terminals
                    for (char t : terminalList) {

                        addAction(
                            i,
                            t,
                            "r" + to_string(p)
                        );
                    }
                }
            }
        }
    }


    // ------------------------------------------------
    // PRINT PARSING TABLE
    // ------------------------------------------------

    cout << "\n\nLR(0) PARSING TABLE\n";
    cout << "================================================\n";

    cout << left << setw(8) << "State";

    for (char t : terminalList)
        cout << setw(12) << t;

    for (char nt : nonTerminalList)
        cout << setw(8) << nt;

    cout << "\n";

    cout << "------------------------------------------------\n";

    for (int i = 0; i < states.size(); i++) {

        cout << setw(8) << i;

        for (char t : terminalList) {

            if (ACTION[i].count(t))
                cout << setw(12) << ACTION[i][t];
            else
                cout << setw(12) << "";
        }

        for (char nt : nonTerminalList) {

            if (GOTO[i].count(nt))
                cout << setw(8) << GOTO[i][nt];
            else
                cout << setw(8) << "";
        }

        cout << "\n";
    }


    // ------------------------------------------------
    // RESULT
    // ------------------------------------------------

    cout << "\nPRODUCTIONS:\n";
    cout << "r1: S -> A\n";
    cout << "r2: A -> Ab\n";
    cout << "r3: A -> a\n";

    if (conflict) {

        cout << "\nGrammar is NOT LR(0)\n";
        cout << "Shift-Reduce conflict detected.\n";

    } else {

        cout << "\nGrammar is LR(0)\n";
    }

    return 0;
}
