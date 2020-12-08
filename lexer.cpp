#include "lexer.hpp"
#include <cctype>
#include <algorithm>

std::vector<Token> Lexer::parse(const std::string& code) {
    for (auto iter = code.begin(); iter != code.end(); ++iter) {
        parsing_automata(*iter);
    }
}

void Lexer::parsing_automata(const char& ch) {
    enum STATE {
        INIT, NUM, ID, OPERATOR, ACCEPT
    };
    static STATE state {STATE::INIT};
    static int lineNumber {0};
    static std::string token;
    auto init_handle = [](const char& ch) {
        token.clear();
        if (isdigit(ch)) {
            token += ch;
            state = STATE::NUM;
        } else if (ispunct(ch)) {
            token += ch;
            state = STATE::OPERATOR;
        } else if (isalpha(ch) || ch == '_') {
            token += ch;
            state = STATE::ID;
        } else if (isblank(ch)) {
            if (ch == '\n') {
                ++lineNumber;
            }
        } else {
            throw std::runtime_error(std::string("Character ") + ch +" is not expected.");
        }
    };
    auto num_handle = [this](const char& ch) {
        if (isdigit(ch)) {
            token += ch;
        } else {
            _token_list.push_back({
                get_terminal_id("<INT>"),
                lineNumber,
                "<INT>",
                token
            });
        }
    };
    auto id_handle = [](const char& ch) {};
    auto op_handle = [](const char& ch) {};

    switch (state) {
        case INIT:
            init_handle(ch);
            break;
        case NUM:
            num_handle(ch);
            break;
        case ID:
            id_handle(ch);
            break;
        case OPERATOR:
            op_handle(ch);
            break;
        default:
            throw std::runtime_error(std::string("Unexpected STATE") + static_cast<char>(state));
    }

    if (state == STATE::ACCEPT) {

    }
}

int Lexer::get_terminal_id(std::string tokenVal) {
    auto iter {std::find(_terminals.begin(), _terminals.end(), tokenVal)};
    if (iter == _terminals.end()) {
        // NOT FOUND
        throw std::runtime_error(std::string("Token ") + tokenVal + " not found");
    } else {
        return iter - _terminals.begin();
    }
}
