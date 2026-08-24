#include <iostream>
#include <vector>
#include <string>

using namespace std;

void removeLR(char nonT, vector<string> production) {
    vector<string> alpha;
    vector<string> beta;

    for (string p : production) {
        if (p[0] == nonT)
            alpha.push_back(p.substr(1));
        else
            beta.push_back(p);
    }

    if (alpha.empty()) {
        cout << nonT << "->";
        for (int i = 0; i < production.size(); i++) {
            cout << production[i];
            if (i != production.size() - 1) cout << "|";
        }
        cout << endl;
        return;
    }

    string newNT = string(1, nonT) + "'";

    cout << nonT << "->";
    for (int i = 0; i < beta.size(); i++) {
        cout << beta[i] << newNT;
        if (i != beta.size() - 1) cout << "|";
    }
    cout << endl;

    cout << newNT << "->";
    for (int i = 0; i < alpha.size(); i++) {
        cout << alpha[i] << newNT << "|";
    }
    cout << "epsilon" << endl;
}

int main() {
    cout << "Original Grammar:\n";
    cout << "E->E+T | T\n";
    cout << "T->T*F | F\n";
    cout << "F->(E) | id\n\n";

    cout << "Grammar after removing left recursion:\n";

    vector<string> E = {"E+T", "T"};
    vector<string> T = {"T*F", "F"};
    vector<string> F = {"(E)", "id"};

    removeLR('E', E);
    removeLR('T', T);
    removeLR('F', F);

    return 0;
}
