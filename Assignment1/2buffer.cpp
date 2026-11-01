
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_set>
using namespace std;

#define N 10

// Two input buffers with one sentinel position each
int buffer[2][N + 1];

int lengthOfBuffer[2] = {0, 0};
bool ready[2] = {false, false};
bool eofAfter[2] = {false, false};

int currentBuffer = 0;
int forward = 0;
int lexemeBegin = 0;
int lexemeBuffer = 0;

bool eofReached = false;

// Previous position for retract operation
int previousBuffer = 0;
int previousPosition = 0;
bool canRetract = false;

ifstream fin("input.txt");

unordered_set<string> keywords = {
    "int", "float", "char", "double", "void",
    "if", "else", "while", "for", "return",
    "break", "continue", "switch", "case",
    "default", "do", "long", "short"
};

// Load characters into a buffer and place sentinel
void loadBuffer(int b) {

    int i = 0;
    char ch;

    while (i < N && fin.get(ch)) {
        buffer[b][i++] = (unsigned char)ch;
    }

    lengthOfBuffer[b] = i;
    eofAfter[b] = (i < N);

    buffer[b][i] = EOF;

    ready[b] = true;
}

// Read the next character using forward pointer
int getNextChar() {

    if (eofReached)
        return EOF;

    // Check whether forward has reached sentinel
    if (forward == lengthOfBuffer[currentBuffer]) {

        // Actual end of file
        if (eofAfter[currentBuffer]) {
            eofReached = true;
            canRetract = false;
            return EOF;
        }

        // Switch to the other buffer
        ready[currentBuffer] = false;

        currentBuffer = 1 - currentBuffer;

        // Load only if this buffer is not already loaded
        if (!ready[currentBuffer])
            loadBuffer(currentBuffer);

        forward = 0;

        // Handle empty buffer at end of file
        if (lengthOfBuffer[currentBuffer] == 0 &&
            eofAfter[currentBuffer]) {
            eofReached = true;
            canRetract = false;
            return EOF;
        }
    }

    // Save previous position for retract
    previousBuffer = currentBuffer;
    previousPosition = forward;
    canRetract = true;

    int ch = buffer[currentBuffer][forward];
    forward++;

    return ch;
}

// Move forward pointer one character backward
void retract() {

    if (canRetract) {
        currentBuffer = previousBuffer;
        forward = previousPosition;
        canRetract = false;
        eofReached = false;
    }
}

// Lookahead without consuming the character
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

// Recognize operators using maximal munch
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

        // Mark beginning of current lexeme
        lexemeBuffer = previousBuffer;
        lexemeBegin = previousPosition;

        string lexeme;

        // 1. Keyword or Identifier
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

        // 2. Number
        else if (isdigit(ch)) {

            lexeme += (char)ch;

            while (isdigit(peekNextChar()))
                lexeme += (char)getNextChar();

            // Optional decimal part
            if (peekNextChar() == '.') {
                lexeme += (char)getNextChar();

                while (isdigit(peekNextChar()))
                    lexeme += (char)getNextChar();
            }

            cout << lexeme << " : NUMBER\n";
        }

        // 3. Operator
        else if (isOperatorChar(ch)) {

            lexeme = getOperator(ch);

            cout << lexeme << " : OPERATOR\n";
        }

        // 4. Punctuation
        else if (string(";,(){}[].:").find((char)ch)
                 != string::npos) {

            cout << (char)ch << " : PUNCTUATION\n";
        }

        // Unrecognized character
        else {
            cout << (char)ch << " : UNKNOWN\n";
        }
    }
}

int main() {

    if (!fin) {
        cout << "Error: Cannot open input.txt\n";
        return 1;
    }

    // Initially load the first buffer
    loadBuffer(0);

    analyze();

    fin.close();

    return 0;
}
