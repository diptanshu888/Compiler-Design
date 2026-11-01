#include <bits/stdc++.h>
using namespace std;

const int BUF_SIZE = 32;
const char SENTINEL = EOF;

const unordered_set<string> KEYWORDS = {
    "int", "float", "double", "char", "if", "else", "while", "for", 
    "return", "void", "include", "using", "namespace", "class"
};

bool isKeyword(const string& str) {
    return KEYWORDS.find(str) != KEYWORDS.end();
}

bool isIdentifier(const string& str) {
    if (str.empty() || !isalpha(str[0]) && str[0] != '_') return false;
    for (char c : str) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

class BufferSystem {
    ifstream fin;
    char b1[BUF_SIZE + 1];
    char b2[BUF_SIZE + 1];
    char* lexemeBegin;
    char* forward;
    bool isB1;

    void load(char* buf) {
        if (!fin.is_open()) return;
        fin.read(buf, BUF_SIZE);
        streamsize n = fin.gcount();
        buf[n] = SENTINEL;
    }

public:
    BufferSystem(const string& filename) {
        fin.open(filename, ios::in | ios::binary);
        if (!fin.is_open()) {
            cerr << "Error opening file\n";
            exit(1);
        }
        isB1 = true;
        memset(b1, 0, sizeof(b1));
        memset(b2, 0, sizeof(b2));
        load(b1);
        lexemeBegin = b1;
        forward = b1;
    }

    ~BufferSystem() {
        if (fin.is_open()) fin.close();
    }

    void parse() {
        while (true) {
            while (*forward == ' ' || *forward == '\t' || *forward == '\n' || *forward == '\r') {
                forward++;
                lexemeBegin = forward;
            }

            if (*forward == SENTINEL) {
                if (forward == b1 + BUF_SIZE) {
                    load(b2);
                    forward = b2;
                    isB1 = false;
                    continue;
                } else if (forward == b2 + BUF_SIZE) {
                    load(b1);
                    forward = b1;
                    isB1 = true;
                    continue;
                } else {
                    break;
                }
            }

            lexemeBegin = forward;
            string lex = "";

            while (true) {
                if (*forward == ' ' || *forward == '\t' || *forward == '\n' || *forward == '\r' || *forward == SENTINEL) {
                    if (*forward == SENTINEL) {
                        if (forward == b1 + BUF_SIZE) {
                            load(b2);
                            forward = b2;
                            isB1 = false;
                            continue;
                        } else if (forward == b2 + BUF_SIZE) {
                            load(b1);
                            forward = b1;
                            isB1 = true;
                            continue;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                
                if (string("(){}[];=<>+-*/,").find(*forward) != string::npos) {
                    if (lex.empty()) {
                        lex += *forward;
                        forward++;
                    }
                    break;
                }

                lex += *forward;
                forward++;
            }

            if (!lex.empty()) {
                if (isKeyword(lex)) {
                    cout << "<KEYWORD, " << lex << ">\n";
                } else if (isIdentifier(lex)) {
                    cout << "<IDENTIFIER, " << lex << ">\n";
                } else if (isdigit(lex[0])) {
                    cout << "<NUMBER, " << lex << ">\n";
                } else {
                    cout << "<OPERATOR/DELIMITER, " << lex << ">\n";
                }
            }
        }
    }
};

int main() {
    string filename = "input.txt";
    BufferSystem bs(filename);
    bs.parse();
    return 0;
}