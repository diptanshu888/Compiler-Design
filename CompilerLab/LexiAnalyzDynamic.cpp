
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <cstdio>
#include <unordered_set>
using namespace std;

#define N 10

int buffer[2][N + 1];
int lengthOfBuffer[2] = {0, 0};
bool ready[2] = {false, false};
bool eofAfter[2] = {false, false};

int currentBuffer = 0;
int fwd = 0;
int lexemeBegin = 0;
int lexemeBuffer = 0;

bool eofReached = false;

int previousBuffer = 0;
int previousPosition = 0;
bool canRetract = false;

ifstream fin;

unordered_set<string> keywords = {
    "int", "float", "char", "double", "void",
    "if", "else", "while", "for", "return",
    "break", "continue", "switch", "case",
    "default", "do", "long", "short"
};

// Load characters and place sentinel
void loadBuffer(int b) {
    int i = 0;
    char ch;

    while (i < N && fin.get(ch))
        buffer[b][i++] = (unsigned char)ch;

    lengthOfBuffer[b] = i;
    eofAfter[b] = (i < N);
    buffer[b][i] = EOF;
    ready[b] = true;
}

// Read next character
int getNextChar() {
    if (eofReached)
        return EOF;

    if (fwd == lengthOfBuffer[currentBuffer]) {
        if (eofAfter[currentBuffer]) {
            eofReached = true;
            canRetract = false;
            return EOF;
        }

        ready[currentBuffer] = false;
        currentBuffer = 1 - currentBuffer;

        if (!ready[currentBuffer])
            loadBuffer(currentBuffer);

        fwd = 0;

        if (lengthOfBuffer[currentBuffer] == 0 &&
            eofAfter[currentBuffer]) {
            eofReached = true;
            canRetract = false;
            return EOF;
        }
    }

    previousBuffer = currentBuffer;
    previousPosition = fwd;
    canRetract = true;

    return buffer[currentBuffer][fwd++];
}

// Move pointer backward
void retract() {
    if (canRetract) {
        currentBuffer = previousBuffer;
        fwd = previousPosition;
        canRetract = false;
        eofReached = false;
    }
}

// Lookahead without consuming
int peekNextChar() {
    int ch = getNextChar();

    if (ch != EOF)
        retract();

    return ch;
}

bool isIdentifierChar(int ch) {
    return isalnum(ch) || ch == '_';
}

bool isOperatorChar(int ch) {
    return string("+-*/%=<>!&|").find((char)ch)
           != string::npos;
}

// Recognize longest possible operator
string getOperator(int first) {
    string op(1, (char)first);
    int next = peekNextChar();

    if (next == '=') {
        op += (char)getNextChar();
    }
    else if ((first == '+' && next == '+') ||
             (first == '-' && next == '-') ||
             (first == '&' && next == '&') ||
             (first == '|' && next == '|')) {
        op += (char)getNextChar();
    }

    return op;
}

// Perform lexical analysis
void analyze() {
    int ch;

    while ((ch = getNextChar()) != EOF) {

        if (isspace(ch))
            continue;

        lexemeBuffer = previousBuffer;
        lexemeBegin = previousPosition;

        string lexeme;

        // Keyword or Identifier
        if (isalpha(ch) || ch == '_') {
            lexeme += (char)ch;

            while (true) {
                int next = peekNextChar();

                if (next != EOF && isIdentifierChar(next))
                    lexeme += (char)getNextChar();
                else
                    break;
            }

            if (keywords.count(lexeme))
                cout << lexeme << " : KEYWORD\n";
            else
                cout << lexeme << " : IDENTIFIER\n";
        }

        // Number
        else if (isdigit(ch)) {
            lexeme += (char)ch;

            while (isdigit(peekNextChar()))
                lexeme += (char)getNextChar();

            if (peekNextChar() == '.') {
                lexeme += (char)getNextChar();

                while (isdigit(peekNextChar()))
                    lexeme += (char)getNextChar();
            }

            cout << lexeme << " : NUMBER\n";
        }

        // Operator
        else if (isOperatorChar(ch)) {
            lexeme = getOperator(ch);
            cout << lexeme << " : OPERATOR\n";
        }

        // Punctuation
        else if (string(";,(){}[].:").find((char)ch)
                 != string::npos) {
            cout << (char)ch << " : PUNCTUATION\n";
        }

        // Unknown character
        else {
            cout << (char)ch << " : UNKNOWN\n";
        }
    }
}

int main() {
    string filename;

    cout << "Enter input file name: ";
    getline(cin, filename);

    fin.open(filename);

    if (!fin) {
        cout << "Error: Cannot open file!\n";
        return 1;
    }

    loadBuffer(0);
    analyze();

    fin.close();

    return 0;
}
