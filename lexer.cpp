#include "lexer.hpp"
#include <cctype>
#include <algorithm>
#include <stdexcept>

const char Lexer::FLUSH;

std::vector<Token> Lexer::parse(const std::string& code) {
    for (const char& iter : code) {
        parsing_automata(iter);
    }
    parsing_automata(FLUSH);
    return _token_list;
}

void Lexer::parsing_automata(char c) {
    enum STATE {
        INIT, NUM, ID, OPERATOR, ACCEPT
    };
    static STATE state {STATE::INIT};
    static int lineNumber {0};
    static std::string token;

    static auto init_accept_handle = [](const char& c) {
        if (isdigit(c)) {
            token += c;
            state = STATE::NUM;
        } else if (ispunct(c)) {
            token += c;
            state = STATE::OPERATOR;
        } else if (isalpha(c) || c == '_') {
            token += c;
            state = STATE::ID;
        } else if (isblank(c)) {
            state = STATE::INIT;
        } else {
            throw std::runtime_error(std::string("Character ") + c + " is not expected.");
        }
    };
    static auto num_handle = [this](const char& ch) {
        if (isdigit(ch)) {
            token += ch;
        } else {
            _token_list.push_back({
                                      get_terminal_id("<INT>"),
                                      lineNumber,
                                      "<INT>",
                                      token
                                  });
            state = STATE::ACCEPT;
        }
    };
    static auto id_handle = [this](const char& ch) {
        if (isalnum(ch) || ch == '_') {
            token += ch;
        } else {
            // First try find the token in the _terminals
            // If found then it's a keyword, otherwise an identifier (variable).
            if (is_valid_terminal(token)) {
                // token is a keyword
                _token_list.push_back({
                                          get_terminal_id(token),
                                          lineNumber,
                                          token,
                                          token
                                      });
            } else {
                // token is a <ID>
                _token_list.push_back({
                                          get_terminal_id("<ID>"),
                                          lineNumber,
                                          "<ID>",
                                          token
                                      });
            }
            state = STATE::ACCEPT;
        }
    };
    static auto op_handle = [this](const char& ch) {
        /**
         * Operators are special, for it ends when extending the next character cannot
         * make the token found in _terminals
         */
        if (ispunct(ch) && is_valid_terminal(token + ch)) {
            token += ch;
        } else {
            _token_list.push_back({
                get_terminal_id(token),
                lineNumber,
                token,
                token
            });
            state = STATE::ACCEPT;
        }
    };

    if (c == FLUSH) {
        c = ' ';
    }

    switch (state) {
        case INIT:
            init_accept_handle(c);
            break;
        case NUM:
            num_handle(c);
            break;
        case ID:
            id_handle(c);
            break;
        case OPERATOR:
            op_handle(c);
            break;
        default:
            throw std::runtime_error(std::string("Unexpected STATE") + static_cast<char>(state));
    }

    if (state == STATE::ACCEPT) {
        token.clear();
        init_accept_handle(c);
    }

    if (c == '\n') {
        ++lineNumber;
    }
}

int Lexer::get_terminal_id(const std::string& tokenVal) {
    auto iter {std::find(_terminals.begin(), _terminals.end(), tokenVal)};
    if (iter == _terminals.end()) {
        // NOT FOUND
        throw std::runtime_error(std::string("Token ") + tokenVal + " not found");
    } else {
        return iter - _terminals.begin();
    }
}

bool Lexer::is_valid_terminal(const std::string& word) {
    return std::find(_terminals.begin(), _terminals.end(), word) != _terminals.end();
}
