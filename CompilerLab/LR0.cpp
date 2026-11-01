#include <bits/stdc++.h>
using namespace std;

string START_SYMBOL = "S";
vector<pair<string, vector<string>>> original_grammar = {
    {"S", {"A"}},
    {"A", {"a"}}
};

struct Production {
    string lhs;
    vector<string> rhs;
};

struct Item {
    int prod_idx;
    int dot_pos;

    bool operator<(const Item& other) const {
        if (prod_idx != other.prod_idx) return prod_idx < other.prod_idx;
        return dot_pos < other.dot_pos;
    }
    bool operator==(const Item& other) const {
        return prod_idx == other.prod_idx && dot_pos == other.dot_pos;
    }
};

vector<Production> grammar;
set<string> non_terminals;
set<string> terminals;
vector<set<Item>> states;
map<pair<int, string>, int> transitions;
map<pair<int, string>, string> action_table;
map<pair<int, string>, int> goto_table;

bool isNonTerminal(const string& sym) {
    return non_terminals.find(sym) != non_terminals.end();
}

set<Item> getClosure(set<Item> items) {
    bool added = true;
    while (added) {
        added = false;
        set<Item> new_items;
        for (const auto& item : items) {
            const auto& rhs = grammar[item.prod_idx].rhs;
            if (item.dot_pos < rhs.size()) {
                string B = rhs[item.dot_pos];
                if (isNonTerminal(B)) {
                    for (size_t i = 0; i < grammar.size(); ++i) {
                        if (grammar[i].lhs == B) {
                            Item cand = {(int)i, 0};
                            if (items.find(cand) == items.end() && new_items.find(cand) == new_items.end()) {
                                new_items.insert(cand);
                                added = true;
                            }
                        }
                    }
                }
            }
        }
        items.insert(new_items.begin(), new_items.end());
    }
    return items;
}

set<Item> getGoto(const set<Item>& items, const string& symbol) {
    set<Item> j;
    for (const auto& item : items) {
        const auto& rhs = grammar[item.prod_idx].rhs;
        if (item.dot_pos < rhs.size() && rhs[item.dot_pos] == symbol) {
            j.insert({item.prod_idx, item.dot_pos + 1});
        }
    }
    return getClosure(j);
}

void generateStates() {
    grammar.push_back({START_SYMBOL + "'", {START_SYMBOL}});
    for (const auto& p : original_grammar) {
        grammar.push_back({p.first, p.second});
        non_terminals.insert(p.first);
    }
    non_terminals.insert(START_SYMBOL + "'");

    for (const auto& p : grammar) {
        for (const string& sym : p.rhs) {
            if (non_terminals.find(sym) == non_terminals.end()) {
                terminals.insert(sym);
            }
        }
    }
    terminals.insert("$");

    set<Item> I0 = getClosure({{0, 0}});
    states.push_back(I0);

    queue<int> q;
    q.push(0);

    set<string> all_symbols;
    for (const string& nt : non_terminals) all_symbols.insert(nt);
    for (const string& t : terminals) all_symbols.insert(t);

    while (!q.empty()) {
        int idx = q.front();
        q.pop();

        for (const string& sym : all_symbols) {
            set<Item> next_state = getGoto(states[idx], sym);
            if (!next_state.empty()) {
                auto it = find(states.begin(), states.end(), next_state);
                int next_idx;
                if (it == states.end()) {
                    states.push_back(next_state);
                    next_idx = states.size() - 1;
                    q.push(next_idx);
                } else {
                    next_idx = distance(states.begin(), it);
                }
                transitions[{idx, sym}] = next_idx;
            }
        }
    }
}

void buildTables() {
    for (size_t i = 0; i < states.size(); ++i) {
        for (const auto& item : states[i]) {
            int p_idx = item.prod_idx;
            int dot = item.dot_pos;
            const auto& rhs = grammar[p_idx].rhs;

            if (dot == rhs.size()) {
                if (p_idx == 0) {
                    action_table[{i, "$"}] = "acc";
                } else {
                    for (const string& t : terminals) {
                        action_table[{i, t}] = "r" + to_string(p_idx);
                    }
                }
            } else {
                string sym = rhs[dot];
                if (terminals.find(sym) != terminals.end()) {
                    if (transitions.count({i, sym})) {
                        action_table[{i, sym}] = "s" + to_string(transitions[{i, sym}]);
                    }
                }
            }
        }

        for (const string& nt : non_terminals) {
            if (nt == START_SYMBOL + "'") continue;
            if (transitions.count({i, nt})) {
                goto_table[{i, nt}] = transitions[{i, nt}];
            }
        }
    }
}

void printParsingTable() {
    cout << "\n==================================================\n";
    cout << "           AUTOMATED LR(0) PARSING TABLE\n";
    cout << "==================================================\n";

    vector<string> term_list;
    for (const string& t : terminals) term_list.push_back(t);

    vector<string> non_term_list;
    for (const string& nt : non_terminals) {
        if (nt != START_SYMBOL + "'") non_term_list.push_back(nt);
    }

    cout << left << setw(8) << "State";
    for (const string& t : term_list) cout << left << setw(10) << t;
    for (const string& nt : non_term_list) cout << left << setw(10) << nt;
    cout << "\n" << string(8 + 10 * (term_list.size() + non_term_list.size()), '-') << "\n";

    for (size_t i = 0; i < states.size(); ++i) {
        cout << left << setw(8) << i;
        for (const string& t : term_list) {
            string entry = "-";
            if (action_table.count({i, t})) entry = action_table[{i, t}];
            cout << left << setw(10) << entry;
        }
        for (const string& nt : non_term_list) {
            string entry = "-";
            if (goto_table.count({i, nt})) entry = to_string(goto_table[{i, nt}]);
            cout << left << setw(10) << entry;
        }
        cout << "\n";
    }
    cout << string(8 + 10 * (term_list.size() + non_term_list.size()), '-') << "\n\n";
}

void parseInput(vector<string> tokens) {
    tokens.push_back("$");
    vector<int> state_stack = {0};
    vector<string> symbol_stack = {};
    int ip = 0;

    cout << "Parsing Input: ";
    for (size_t i = 0; i < tokens.size() - 1; ++i) cout << tokens[i] << " ";
    cout << "\n\n";

    cout << left << setw(20) << "STATE STACK" << setw(20) << "SYMBOL STACK" << setw(20) << "INPUT" << "ACTION\n";
    cout << string(75, '-') << "\n";

    while (true) {
        int state = state_stack.back();
        string lookahead = tokens[ip];

        string s_str = ""; for (int s : state_stack) s_str += to_string(s) + " ";
        string sym_str = ""; for (const string& sym : symbol_stack) sym_str += sym + " ";
        string in_str = ""; for (size_t k = ip; k < tokens.size(); ++k) in_str += tokens[k] + " ";

        cout << left << setw(20) << s_str << setw(20) << sym_str << setw(20) << in_str;

        if (action_table.count({state, lookahead}) == 0) {
            cout << "ERROR\n";
            break;
        }

        string action = action_table[{state, lookahead}];
        if (action == "acc") {
            cout << "ACCEPT SUCCESSFUL\n";
            break;
        } else if (action[0] == 's') {
            int next_state = stoi(action.substr(1));
            cout << "Shift to State " << next_state << "\n";
            state_stack.push_back(next_state);
            symbol_stack.push_back(lookahead);
            ip++;
        } else if (action[0] == 'r') {
            int p_idx = stoi(action.substr(1));
            const auto& prod = grammar[p_idx];
            cout << "Reduce by " << prod.lhs << " -> ";
            for (const string& s : prod.rhs) cout << s;
            cout << "\n";

            for (size_t k = 0; k < prod.rhs.size(); ++k) {
                state_stack.pop_back();
                symbol_stack.pop_back();
            }

            int top_state = state_stack.back();
            state_stack.push_back(goto_table[{top_state, prod.lhs}]);
            symbol_stack.push_back(prod.lhs);
        }
    }
    cout << "\n";
}

int main() {
    generateStates();
    buildTables();

    cout << "==================================================\n";
    cout << "     AUTOMATED LR(0) PARSER & TABLE GENERATOR\n";
    cout << "==================================================\n";
    cout << "Total LR(0) States Generated: " << states.size() << "\n";

    printParsingTable();

    parseInput({"a"});
    parseInput({"a", "b", "b"});

    return 0;
}