#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <iomanip>

// Token Types
enum TokenType {
    TOKEN_NUMBER,
    TOKEN_ID,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF
};

struct Token {
    TokenType type;
    std::string value;
};

// Quadruple structure: (Op, Arg1, Arg2, Result)
struct Quadruple {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

// Triple structure: (Op, Arg1, Arg2)
struct Triple {
    std::string op;
    std::string arg1;
    std::string arg2;
};

// Lexer Class
class Lexer {
private:
    std::string text;
    size_t pos;
    char current_char;

    void advance() {
        pos++;
        if (pos < text.length()) {
            current_char = text[pos];
        } else {
            current_char = '\0';
        }
    }

    void skip_whitespace() {
        while (current_char != '\0' && std::isspace(current_char)) {
            advance();
        }
    }

    std::string number() {
        std::string result = "";
        while (current_char != '\0' && (std::isdigit(current_char) || current_char == '.')) {
            result += current_char;
            advance();
        }
        return result;
    }

    std::string identifier() {
        std::string result = "";
        while (current_char != '\0' && (std::isalnum(current_char) || current_char == '_')) {
            result += current_char;
            advance();
        }
        return result;
    }

public:
    Lexer(std::string input) : text(input), pos(0) {
        current_char = text.empty() ? '\0' : text[0];
    }

    Token get_next_token() {
        while (current_char != '\0') {
            if (std::isspace(current_char)) {
                skip_whitespace();
                continue;
            }

            if (std::isalpha(current_char) || current_char == '_') {
                return {TOKEN_ID, identifier()};
            }

            if (std::isdigit(current_char)) {
                return {TOKEN_NUMBER, number()};
            }

            if (current_char == '=') { advance(); return {TOKEN_ASSIGN, "="}; }
            if (current_char == '+') { advance(); return {TOKEN_PLUS, "+"}; }
            if (current_char == '-') { advance(); return {TOKEN_MINUS, "-"}; }
            if (current_char == '*') { advance(); return {TOKEN_MUL, "*"}; }
            if (current_char == '/') { advance(); return {TOKEN_DIV, "/"}; }
            if (current_char == '(') { advance(); return {TOKEN_LPAREN, "("}; }
            if (current_char == ')') { advance(); return {TOKEN_RPAREN, ")"}; }

            throw std::runtime_error(std::string("Unexpected character: ") + current_char);
        }

        return {TOKEN_EOF, ""};
    }
};

// Parser Class & TAC Generator
class Parser {
private:
    Lexer lexer;
    Token current_token;
    int temp_count;

    void eat(TokenType token_type) {
        if (current_token.type == token_type) {
            current_token = lexer.get_next_token();
        } else {
            throw std::runtime_error("Syntax Error: Unexpected token");
        }
    }

    std::string new_temp() {
        return "t" + std::to_string(temp_count++);
    }

    std::string factor() {
        Token token = current_token;
        if (token.type == TOKEN_NUMBER) {
            eat(TOKEN_NUMBER);
            return token.value;
        } else if (token.type == TOKEN_ID) {
            eat(TOKEN_ID);
            return token.value;
        } else if (token.type == TOKEN_LPAREN) {
            eat(TOKEN_LPAREN);
            std::string node = expression();
            eat(TOKEN_RPAREN);
            return node;
        }
        throw std::runtime_error("Invalid syntax in factor");
    }

    std::string term() {
        std::string node = factor();
        while (current_token.type == TOKEN_MUL || current_token.type == TOKEN_DIV) {
            Token token = current_token;
            std::string op = (token.type == TOKEN_MUL) ? "*" : "/";
            eat(token.type);
            std::string right = factor();
            std::string temp = new_temp();
            tac.push_back({op, node, right, temp});
            node = temp;
        }
        return node;
    }

    std::string expression() {
        std::string node = term();
        while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
            Token token = current_token;
            std::string op = (token.type == TOKEN_PLUS) ? "+" : "-";
            eat(token.type);
            std::string right = term();
            std::string temp = new_temp();
            tac.push_back({op, node, right, temp});
            node = temp;
        }
        return node;
    }

public:
    std::vector<Quadruple> tac; // Using Quadruple structure to store TAC (op, arg1, arg2, res)

    Parser(Lexer l) : lexer(l), temp_count(1) {
        current_token = lexer.get_next_token();
    }

    std::string parse() {
        // Check for assignment statement: ID = expression
        if (current_token.type == TOKEN_ID) {
            // Peek ahead manually if needed, or save state. For simplicity:
            std::string id_val = current_token.value;
            eat(TOKEN_ID);
            if (current_token.type == TOKEN_ASSIGN) {
                eat(TOKEN_ASSIGN);
                std::string expr_res = expression();
                tac.push_back({"=", expr_res, "", id_val});
                return id_val;
            } else {
                // If it wasn't an assignment, roll back logic or handle error/expression
                // For a robust implementation, we check before consuming.
            }
        }
        return expression();
    }
};

// Helper function to check if a string is a temporary variable
bool is_temp(const std::string& str) {
    return !str.empty() && str[0] == 't' && std::isdigit(str[1]);
}

int main() {
    std::string expression = "a = b + c * d";
    std::cout << "Input Expression: " << expression << "\n\n";

    try {
        Lexer lexer(expression);
        Parser parser(lexer);
        parser.parse();

        // 1. Print Three-Address Code (TAC)
        std::cout << "--- Three-Address Code (TAC) ---\n";
        for (const auto& quad : parser.tac) {
            if (quad.op == "=") {
                std::cout << quad.result << " = " << quad.arg1 << "\n";
            } else {
                std::cout << quad.result << " = " << quad.arg1 << " " << quad.op << " " << quad.arg2 << "\n";
            }
        }

        // 2. Generate Quadruples and Triples
        std::vector<Quadruple> quadruples;
        std::vector<Triple> triples;
        std::unordered_map<std::string, int> temp_to_index;

        for (size_t i = 0; i < parser.tac.size(); ++i) {
            const auto& q = parser.tac[i];
            
            // Quadruple entry
            quadruples.push_back({q.op, q.arg1, q.arg2, q.result});

            // Triple argument resolution
            std::string t_arg1 = (temp_to_index.find(q.arg1) != temp_to_index.end()) ? 
                                 "(" + std::to_string(temp_to_index[q.arg1]) + ")" : q.arg1;
            std::string t_arg2 = (!q.arg2.empty() && temp_to_index.find(q.arg2) != temp_to_index.end()) ? 
                                 "(" + std::to_string(temp_to_index[q.arg2]) + ")" : q.arg2;

            if (q.op == "=") {
                triples.push_back({q.op, t_arg1, ""});
            } else {
                triples.push_back({q.op, t_arg1, t_arg2});
            }

            if (!q.result.empty() && is_temp(q.result)) {
                temp_to_index[q.result] = i;
            }
        }

        // Print Quadruples
        std::cout << "\n--- Quadruple Representation ---\n";
        std::cout << std::left << std::setw(6) << "Index" 
                  << std::setw(6) << "Op" 
                  << std::setw(8) << "Arg1" 
                  << std::setw(8) << "Arg2" 
                  << std::setw(8) << "Result" << "\n";
        std::cout << "------------------------------------------\n";
        for (size_t i = 0; i < quadruples.size(); ++i) {
            std::cout << std::left << std::setw(6) << i
                      << std::setw(6) << quadruples[i].op
                      << std::setw(8) << quadruples[i].arg1
                      << std::setw(8) << (quadruples[i].arg2.empty() ? "-" : quadruples[i].arg2)
                      << std::setw(8) << quadruples[i].result << "\n";
        }

        // Print Triples
        std::cout << "\n--- Triple Representation ---\n";
        std::cout << std::left << std::setw(6) << "Index" 
                  << std::setw(6) << "Op" 
                  << std::setw(8) << "Arg1" 
                  << std::setw(8) << "Arg2" << "\n";
        std::cout << "------------------------------\n";
        for (size_t i = 0; i < triples.size(); ++i) {
            std::cout << std::left << std::setw(6) << i
                      << std::setw(6) << triples[i].op
                      << std::setw(8) << triples[i].arg1
                      << std::setw(8) << (triples[i].arg2.empty() ? "-" : triples[i].arg2) << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
