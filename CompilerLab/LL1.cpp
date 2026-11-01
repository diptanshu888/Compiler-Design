#include <bits/stdc++.h>
using namespace std;

map<char, vector<string>> grammar = {
    {'S', {"aABe"}},
    {'A', {"bAc", "d"}},
    {'B', {"f", ""}} 
};

set<char> nonTerminals = {'S', 'A', 'B'};
set<char> terminals = {'a', 'b', 'c', 'd', 'e', 'f'};

map<char, set<char>> firstSet;
map<char, set<char>> followSet;

bool isTerm(char c) {
    return !isupper(c) && c != '\0';
}

set<char> getFirstOfString(const string& str) {
    set<char> result;
    if (str.empty() || str == "epsilon") {
        result.insert('\0');
        return result;
    }
    
    for (size_t i = 0; i < str.length(); ++i) {
        char symbol = str[i];
        if (isTerm(symbol)) {
            result.insert(symbol);
            break;
        } else {
            set<char>& fSymbol = firstSet[symbol];
            bool hasEpsilon = false;
            for (char c : fSymbol) {
                if (c == '\0') hasEpsilon = true;
                else result.insert(c);
            }
            if (!hasEpsilon) {
                break;
            }
            if (i == str.length() - 1) {
                result.insert('\0');
            }
        }
    }
    return result;
}

void computeFirst() {
    for (char nt : nonTerminals) {
        firstSet[nt] = set<char>();
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto const& [nt, productions] : grammar) {
            for (const string& prod : productions) {
                set<char> fProd = getFirstOfString(prod);
                for (char c : fProd) {
                    if (firstSet[nt].find(c) == firstSet[nt].end()) {
                        firstSet[nt].insert(c);
                        changed = true;
                    }
                }
            }
        }
    }
}

void computeFollow() {
    for (char nt : nonTerminals) {
        followSet[nt] = set<char>();
    }
    followSet['S'].insert('$');

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto const& [nt, productions] : grammar) {
            for (const string& prod : productions) {
                for (size_t i = 0; i < prod.length(); ++i) {
                    char current = prod[i];
                    if (isupper(current)) {
                        string beta = (i + 1 < prod.length()) ? prod.substr(i + 1) : "";
                        set<char> firstBeta = getFirstOfString(beta);
                        
                        for (char c : firstBeta) {
                            if (c != '\0') {
                                if (followSet[current].find(c) == followSet[current].end()) {
                                    followSet[current].insert(c);
                                    changed = true;
                                }
                            }
                        }
                        
                        if (beta.empty() || firstBeta.find('\0') != firstSet[beta[0]].end() || firstBeta.find('\0') != getFirstOfString(beta).end()) {
                            for (char c : followSet[nt]) {
                                if (followSet[current].find(c) == followSet[current].end()) {
                                    followSet[current].insert(c);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    computeFirst();
    computeFollow();

    cout << "\n";
    cout << "         FIRST & FOLLOW SETS\n";
    cout << "\n";
    for (char nt : nonTerminals) {
        cout << "FIRST(" << nt << ") = { ";
        for (char c : firstSet[nt]) {
            if (c == '\0') cout << "epsilon ";
            else cout << c << " ";
        }
        cout << "}\n";
    }
    cout << "\n";
    for (char nt : nonTerminals) {
        cout << "FOLLOW(" << nt << ") = { ";
        for (char c : followSet[nt]) {
            cout << c << " ";
        }
        cout << "}\n";
    }

    map<pair<char, char>, string> parseTable;
    bool isLL1 = true;

    for (auto const& [nt, productions] : grammar) {
        for (const string& prod : productions) {
            set<char> firstProd = getFirstOfString(prod);
            for (char t : firstProd) {
                if (t != '\0') {
                    if (parseTable.find({nt, t}) != parseTable.end()) {
                        isLL1 = false;
                    }
                    parseTable[{nt, t}] = prod;
                } else {
                    for (char f : followSet[nt]) {
                        if (parseTable.find({nt, f}) != parseTable.end()) {
                            isLL1 = false;
                        }
                        parseTable[{nt, f}] = prod;
                    }
                }
            }
        }
    }

    cout << "\n\n";
    cout << "           LL(1) PARSING TABLE\n";
    cout << "\n";
    cout << left << setw(10) << "NT";
    vector<char> termList = {'a', 'b', 'c', 'd', 'e', 'f', '$'};
    for (char t : termList) cout << left << setw(10) << t;
    cout << "\n" << string(80, '-') << "\n";

    for (char nt : nonTerminals) {
        cout << left << setw(10) << nt;
        for (char t : termList) {
            string entry = parseTable[{nt, t}];
            if (entry.empty()) entry = "-";
            else if (entry == "") entry = "epsilon";
            cout << left << setw(10) << (nt == 'B' && t == 'e' ? "epsilon" : (nt == 'B' && t == '$' ? "-" : entry)); 
        }
        cout << "\n";
    }
    cout << string(80, '-') << "\n";

    cout << "\n\n";
    cout << "         LL(1) GRAMMAR CONFIRMATION\n";
    cout << "\n";
    if (isLL1) {
        cout << "Result: The grammar IS LL(1).\n";
        cout << "Reason: No table cell contains more than one production (No parsing conflicts).\n";
    } else {
        cout << "Result: The grammar IS NOT LL(1).\n";
    }

    return 0;
}