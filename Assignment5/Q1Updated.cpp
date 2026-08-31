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



            if (d < (int)g[p].second.size()) {

                char c = g[p].second[d];



                if (nt(c)) {

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



set<Item> go(set<Item> s, char c) {

    set<Item> t;

    for (auto x : s) {

        int p = x.first;
        int d = x.second;



        if (d < (int)g[p].second.size() &&
            g[p].second[d] == c) {

            t.insert({p, d + 1});
        }
    }

    return closure(t);
}


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



string item(Item x) {

    int p = x.first;
    int d = x.second;

    string r;

    r += g[p].first;
    r += " -> ";


    for (int i = 0; i <= (int)g[p].second.size(); i++) {


        if (i == d)
            r += ".";



        if (i < (int)g[p].second.size())
            r += g[p].second[i];
    }

    return r;
}



void show(set<Item> s, int id) {

    cout << "I" << id << ":\n";

    for (auto x : s) {

        cout << "    "
             << item(x)
             << "\n";
    }
}



string getAction(map<char, string>& a, char c) {

    if (a.count(c))
        return a[c];

    return "-";
}




string getGoto(map<char, int>& h, char c) {

    if (h.count(c))
        return to_string(h[c]);

    return "-";
}



void addAction(
    map<char, string>& action,
    char symbol,
    string newAction,
    int state
) {



    if (!action.count(symbol)) {

        action[symbol] = newAction;
        return;
    }


    string oldAction = action[symbol];



    if (oldAction == newAction)
        return;



    bool oldShift  = oldAction[0] == 's';
    bool newShift  = newAction[0] == 's';

    bool oldReduce = oldAction[0] == 'r';
    bool newReduce = newAction[0] == 'r';




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

 

        if (oldShift) {
            cout << "Resolution       : SHIFT chosen\n";
        }
        else {
        action[symbol] = newAction;

            cout << "Resolution       : SHIFT chosen\n";
        }

        cout << "==============================================\n";

        return;
    }


    if (oldReduce && newReduce) {

        cout << "\n";
        cout << "==============================================\n";
        cout << "        REDUCE/REDUCE CONFLICT DETECTED\n";
        cout << "==============================================\n";

        cout << "State            : " << state << "\n";
        cout << "Input symbol     : " << symbol << "\n";
        cout << "Existing action  : " << oldAction << "\n";
        cout << "New action       : " << newAction << "\n";



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




int main() {



    set<Item> start = closure({{0, 0}});

    vector<set<Item>> states;

    map<string, int> id;


    states.push_back(start);
    id[key(start)] = 0;



    for (int i = 0; i < (int)states.size(); i++) {

        set<char> symbols;




        for (auto x : states[i]) {

            int p = x.first;
            int d = x.second;


            if (d < (int)g[p].second.size()) {

                symbols.insert(
                    g[p].second[d]
                );
            }
        }



        for (char c : symbols) {

            set<Item> next = go(states[i], c);


            if (next.empty())
                continue;


            string k = key(next);



            if (!id.count(k)) {

                int newState = states.size();

                id[k] = newState;

                states.push_back(next);
            }
        }
    }



    cout << "\n";
    cout << "==============================================\n";
    cout << "       CANONICAL COLLECTION OF LR(0) ITEMS\n";
    cout << "==============================================\n\n";


    for (int i = 0; i < (int)states.size(); i++) {

        show(states[i], i);

        cout << "\n";
    }



    vector<map<char, string>> action(states.size());

    vector<map<char, int>> goTo(states.size());


    for (int i = 0; i < (int)states.size(); i++) {

        set<char> symbols;



        for (auto x : states[i]) {

            int p = x.first;
            int d = x.second;


            if (d < (int)g[p].second.size()) {

                symbols.insert(
                    g[p].second[d]
                );
            }
        }



        for (char c : symbols) {

            set<Item> next = go(states[i], c);


            if (next.empty())
                continue;


            int j = id[key(next)];



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



        for (auto x : states[i]) {

            int p = x.first;
            int d = x.second;




            if (d == (int)g[p].second.size()) {

                if (p == 0) {

                    addAction(
                        action[i],
                        '$',
                        "acc",
                        i
                    );
                }

                else {

                    string reduceAction ="r" + to_string(p);

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

    cout << "\nProductions:\n";

    cout << "r1 = S -> A\n";
    cout << "r2 = A -> Ab\n";
    cout << "r3 = A -> a\n";

    string input = "abb$";

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


        int state = stoi(st.back());



        char c = input[pos];



        string act = getAction(
            action[state],
            c
        );

        string stackText;


        for (auto x : st) {

            if (!stackText.empty())
                stackText += " ";

            stackText += x;
        }

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

            st.push_back(
                string(1, c)
            );

            st.push_back(
                to_string(nextState)
            );

            pos++;
        }

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

            for (int j = 0;
                 j < 2 * (int)rhs.size();
                 j++) {

                st.pop_back();
            }

            state =
                stoi(st.back());

            st.push_back(
                string(1, lhs)
            );

            st.push_back(
                to_string(goTo[state][lhs])
            );
        }
    }


    cout << "+----------------------+----------+----------------+\n";


    cout << "\nString accepted successfully.\n";


    return 0;
}
