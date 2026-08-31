#include <bits/stdc++.h>
using namespace std;



using Item = pair<int, int>;




vector<pair<char, string>> g = {
    {'Z', "S"},
    {'S', "A"},
    {'A', "Ab"},
    {'A', "a"}
};




bool nt(char c) {
    return c == 'Z' || c == 'S' || c == 'A';
}



set<Item> closure(set<Item> s) {

    bool changed = true;

    while (changed) {

        changed = false;

        for (auto x : s) {

            int p = x.first;      
            int d = x.second;     


            // Check whether dot is before a symbol
            if (d < (int)g[p].second.size()) {

                char c = g[p].second[d];


                // If symbol after dot is a non-terminal
                if (nt(c)) {

                    // Add all productions of that non-terminal
                    for (int i = 0; i < (int)g.size(); i++) {

                        if (g[i].first == c) {

                            Item y = {i, 0};

                            if (!s.count(y)) {

                                s.insert(y);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }

    return s;
}


// ---------------------------------------------------------
// GOTO
// ---------------------------------------------------------

set<Item> go(set<Item> s, char c) {

    set<Item> t;

    for (auto x : s) {

        int p = x.first;
        int d = x.second;


        // If dot is before c
        if (d < (int)g[p].second.size() &&
            g[p].second[d] == c) {

            // Move dot one position to the right
            t.insert({p, d + 1});
        }
    }

    return closure(t);
}


// ---------------------------------------------------------
// CREATE UNIQUE KEY FOR ITEM SET
// ---------------------------------------------------------

string key(set<Item> s) {

    string r;

    for (auto x : s) {

        r += to_string(x.first);
        r += ",";
        r += to_string(x.second);
        r += ";";
    }

    return r;
}


// ---------------------------------------------------------
// CONVERT ITEM INTO STRING
// ---------------------------------------------------------

string item(Item x) {

    int p = x.first;
    int d = x.second;

    string r;

    r += g[p].first;
    r += " -> ";


    for (int i = 0; i <= (int)g[p].second.size(); i++) {

        // Print dot
        if (i == d)
            r += ".";


        // Print grammar symbol
        if (i < (int)g[p].second.size())
            r += g[p].second[i];
    }

    return r;
}


// ---------------------------------------------------------
// DISPLAY ITEM SET
// ---------------------------------------------------------

void show(set<Item> s, int id) {

    cout << "I" << id << ":\n";

    for (auto x : s) {

        cout << "    "
             << item(x)
             << "\n";
    }
}


// ---------------------------------------------------------
// GET ACTION
// ---------------------------------------------------------

string getAction(map<char, string>& a, char c) {

    if (a.count(c))
        return a[c];

    return "-";
}


// ---------------------------------------------------------
// GET GOTO
// ---------------------------------------------------------

string getGoto(map<char, int>& h, char c) {

    if (h.count(c))
        return to_string(h[c]);

    return "-";
}


// ---------------------------------------------------------
// ADD ACTION + DETECT CONFLICT
// ---------------------------------------------------------

void addAction(
    map<char, string>& action,
    char symbol,
    string newAction,
    int state
) {

    /*
        If there is no action already,
        simply insert the new action.
    */

    if (!action.count(symbol)) {

        action[symbol] = newAction;
        return;
    }


    string oldAction = action[symbol];


    /*
        Same action is not a conflict.
    */

    if (oldAction == newAction)
        return;


    /*
        Determine whether the actions are
        SHIFT or REDUCE.
    */

    bool oldShift  = oldAction[0] == 's';
    bool newShift  = newAction[0] == 's';

    bool oldReduce = oldAction[0] == 'r';
    bool newReduce = newAction[0] == 'r';


    // -----------------------------------------------------
    // SHIFT / REDUCE CONFLICT
    // -----------------------------------------------------

    if ((oldShift && newReduce) ||
        (oldReduce && newShift)) {

        cout << "\n";
        cout << "==============================================\n";
        cout << "       SHIFT/REDUCE CONFLICT DETECTED\n";
        cout << "==============================================\n";

        cout << "State            : " << state << "\n";
        cout << "Input symbol     : " << symbol << "\n";
        cout << "Existing action  : " << oldAction << "\n";
        cout << "New action       : " << newAction << "\n";

        /*
            Conflict resolution:

            SHIFT is preferred.
        */

        if (oldShift) {

            // Already have SHIFT.
            // Keep it.

            cout << "Resolution       : SHIFT chosen\n";
        }
        else {

            // New action is SHIFT.
            // Replace REDUCE with SHIFT.

            action[symbol] = newAction;

            cout << "Resolution       : SHIFT chosen\n";
        }

        cout << "==============================================\n";

        return;
    }


    // -----------------------------------------------------
    // REDUCE / REDUCE CONFLICT
    // -----------------------------------------------------

    if (oldReduce && newReduce) {

        cout << "\n";
        cout << "==============================================\n";
        cout << "        REDUCE/REDUCE CONFLICT DETECTED\n";
        cout << "==============================================\n";

        cout << "State            : " << state << "\n";
        cout << "Input symbol     : " << symbol << "\n";
        cout << "Existing action  : " << oldAction << "\n";
        cout << "New action       : " << newAction << "\n";


        /*
            For demonstration:

            Choose the production with
            the smaller production number.
        */

        int oldProduction =
            stoi(oldAction.substr(1));

        int newProduction =
            stoi(newAction.substr(1));


        if (newProduction < oldProduction) {

            action[symbol] = newAction;
        }

        cout << "Resolution       : Smaller production chosen\n";

        cout << "==============================================\n";

        return;
    }


    // -----------------------------------------------------
    // OTHER CONFLICT
    // -----------------------------------------------------

    cout << "\n";
    cout << "==============================================\n";
    cout << "              CONFLICT DETECTED\n";
    cout << "==============================================\n";

    cout << "State            : " << state << "\n";
    cout << "Input symbol     : " << symbol << "\n";
    cout << "Existing action  : " << oldAction << "\n";
    cout << "New action       : " << newAction << "\n";

    cout << "==============================================\n";
}


// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------

int main() {

    // =====================================================
    // STEP 1: CONSTRUCT CANONICAL COLLECTION
    // =====================================================

    set<Item> start = closure({{0, 0}});

    vector<set<Item>> states;

    map<string, int> id;


    states.push_back(start);
    id[key(start)] = 0;


    /*
        Generate all LR(0) states.
    */

    for (int i = 0; i < (int)states.size(); i++) {

        set<char> symbols;


        /*
            Find all symbols appearing immediately
            after a dot.
        */

        for (auto x : states[i]) {

            int p = x.first;
            int d = x.second;


            if (d < (int)g[p].second.size()) {

                symbols.insert(
                    g[p].second[d]
                );
            }
        }


        /*
            Compute GOTO for every symbol.
        */

        for (char c : symbols) {

            set<Item> next = go(states[i], c);


            if (next.empty())
                continue;


            string k = key(next);


            /*
                If this item set has not appeared before,
                create a new state.
            */

            if (!id.count(k)) {

                int newState = states.size();

                id[k] = newState;

                states.push_back(next);
            }
        }
    }


    // =====================================================
    // STEP 2: DISPLAY LR(0) ITEM SETS
    // =====================================================

    cout << "\n";
    cout << "==============================================\n";
    cout << "       CANONICAL COLLECTION OF LR(0) ITEMS\n";
    cout << "==============================================\n\n";


    for (int i = 0; i < (int)states.size(); i++) {

        show(states[i], i);

        cout << "\n";
    }


    // =====================================================
    // STEP 3: CONSTRUCT PARSING TABLE
    // =====================================================

    vector<map<char, string>> action(states.size());

    vector<map<char, int>> goTo(states.size());


    for (int i = 0; i < (int)states.size(); i++) {

        set<char> symbols;


        /*
            Collect symbols appearing after dots.
        */

        for (auto x : states[i]) {

            int p = x.first;
            int d = x.second;


            if (d < (int)g[p].second.size()) {

                symbols.insert(
                    g[p].second[d]
                );
            }
        }


        // -------------------------------------------------
        // SHIFT AND GOTO
        // -------------------------------------------------

        for (char c : symbols) {

            set<Item> next = go(states[i], c);


            if (next.empty())
                continue;


            int j = id[key(next)];


            /*
                Non-terminal -> GOTO
                Terminal     -> SHIFT
            */

            if (nt(c)) {

                goTo[i][c] = j;
            }
            else {

                addAction(
                    action[i],
                    c,
                    "s" + to_string(j),
                    i
                );
            }
        }


        // -------------------------------------------------
        // REDUCE / ACCEPT
        // -------------------------------------------------

        for (auto x : states[i]) {

            int p = x.first;
            int d = x.second;


            /*
                Dot at the end means
                completed production.
            */

            if (d == (int)g[p].second.size()) {


                // -----------------------------------------
                // ACCEPT
                // -----------------------------------------

                if (p == 0) {

                    addAction(
                        action[i],
                        '$',
                        "acc",
                        i
                    );
                }


                // -----------------------------------------
                // REDUCE
                // -----------------------------------------

                else {

                    string reduceAction =
                        "r" + to_string(p);


                    /*
                        In LR(0), reduction is placed
                        under every terminal.
                    */

                    for (char c : {'a', 'b', '$'}) {

                        addAction(
                            action[i],
                            c,
                            reduceAction,
                            i
                        );
                    }
                }
            }
        }
    }


    // =====================================================
    // STEP 4: DISPLAY PARSING TABLE
    // =====================================================

    cout << "\n";
    cout << "==============================================\n";
    cout << "             LR(0) PARSING TABLE\n";
    cout << "==============================================\n\n";


    cout << "+-------+----------+----------+----------+----------+----------+\n";

    cout << "| State |    a     |    b     |    $     |    S     |    A     |\n";

    cout << "+-------+----------+----------+----------+----------+----------+\n";


    for (int i = 0; i < (int)states.size(); i++) {

        cout << "| "
             << setw(5)
             << i
             << " | "

             << setw(8)
             << getAction(action[i], 'a')
             << " | "

             << setw(8)
             << getAction(action[i], 'b')
             << " | "

             << setw(8)
             << getAction(action[i], '$')
             << " | "

             << setw(8)
             << getGoto(goTo[i], 'S')
             << " | "

             << setw(8)
             << getGoto(goTo[i], 'A')
             << " |\n";
    }


    cout << "+-------+----------+----------+----------+----------+----------+\n";


    // =====================================================
    // STEP 5: DISPLAY PRODUCTIONS
    // =====================================================

    cout << "\nProductions:\n";

    cout << "r1 = S -> A\n";
    cout << "r2 = A -> Ab\n";
    cout << "r3 = A -> a\n";


    // =====================================================
    // STEP 6: PARSE INPUT STRING
    // =====================================================

    string input = "abb$";


    /*
        Stack contains alternating:

        state symbol state symbol state ...

        Initially:

        0
    */

    vector<string> st;

    st.push_back("0");


    int pos = 0;


    cout << "\n";
    cout << "==============================================\n";
    cout << "              PARSING INPUT: abb\n";
    cout << "==============================================\n\n";


    cout << "+----------------------+----------+----------------+\n";

    cout << "| Stack                | Input    | Action         |\n";

    cout << "+----------------------+----------+----------------+\n";


    while (true) {

        // Current state
        int state = stoi(st.back());


        // Current input symbol
        char c = input[pos];


        // Get ACTION[state, input]
        string act = getAction(
            action[state],
            c
        );


        // -------------------------------------------------
        // DISPLAY STACK
        // -------------------------------------------------

        string stackText;


        for (auto x : st) {

            if (!stackText.empty())
                stackText += " ";

            stackText += x;
        }


        // -------------------------------------------------
        // ACCEPT
        // -------------------------------------------------

        if (act == "acc") {

            cout << "| "
                 << left
                 << setw(20)
                 << stackText

                 << " | "
                 << setw(8)
                 << input.substr(pos)

                 << " | "
                 << setw(14)
                 << "ACCEPT"

                 << " |\n";

            break;
        }


        // -------------------------------------------------
        // ERROR
        // -------------------------------------------------

        if (act == "-") {

            cout << "| "
                 << left
                 << setw(20)
                 << stackText

                 << " | "
                 << setw(8)
                 << input.substr(pos)

                 << " | "
                 << setw(14)
                 << "ERROR"

                 << " |\n";

            break;
        }


        // -------------------------------------------------
        // SHIFT
        // -------------------------------------------------

        if (act[0] == 's') {

            int nextState =
                stoi(act.substr(1));


            string displayAction =
                "Shift " + string(1, c);


            cout << "| "
                 << left
                 << setw(20)
                 << stackText

                 << " | "
                 << setw(8)
                 << input.substr(pos)

                 << " | "
                 << setw(14)
                 << displayAction

                 << " |\n";


            /*
                Push terminal.
            */

            st.push_back(
                string(1, c)
            );


            /*
                Push next state.
            */

            st.push_back(
                to_string(nextState)
            );


            /*
                Move input pointer.
            */

            pos++;
        }


        // -------------------------------------------------
        // REDUCE
        // -------------------------------------------------

        else if (act[0] == 'r') {

            int p =
                stoi(act.substr(1));


            char lhs =
                g[p].first;


            string rhs =
                g[p].second;


            string displayAction =
                "Reduce " +
                string(1, lhs) +
                "->" +
                rhs;


            cout << "| "
                 << left
                 << setw(20)
                 << stackText

                 << " | "
                 << setw(8)
                 << input.substr(pos)

                 << " | "
                 << setw(14)
                 << displayAction

                 << " |\n";


            /*
                RHS length = n

                For every grammar symbol,
                stack contains:

                    symbol + state

                Therefore remove:

                    2 * n

                entries.
            */

            for (int j = 0;
                 j < 2 * (int)rhs.size();
                 j++) {

                st.pop_back();
            }


            /*
                Get state now on top.
            */

            state =
                stoi(st.back());


            /*
                Push LHS non-terminal.
            */

            st.push_back(
                string(1, lhs)
            );


            /*
                Push GOTO state.
            */

            st.push_back(
                to_string(goTo[state][lhs])
            );
        }
    }


    cout << "+----------------------+----------+----------------+\n";


    cout << "\nString accepted successfully.\n";


    return 0;
}
