#ifndef LEXER_HPP
#define LEXER_HPP

#include <utility>
#include <vector>
#include <string>
#include <set>
#include <list>

/**
 * @brief 词法分析器 token, 比如 { 0, 10, "int", "int" }, { 30, 1, "<INT>", "45" }
 */
struct Token {
    // 唯一标识符, 根据 Terminal 中的排序得到
    int id;
    // 行号，为后续开发做准备
    int line_number;
    // 当前 token 在 Terminal 集中的值
    std::string name;
    // 真正在输入程序中的表示
    std::string value;
};

/**
 * @brief 词法分析器 Lexer
 * @param terminals: 终结符表
 */
class Lexer {
    // std::vector<token> 类型，词法分析结束后的 token 存放位置
    std::vector<Token> _token_list;
    /**
     * Example:
     *  const std::vector<string> Terminals = {
     *      "int", "float", "double", "void", "if", "else",
     *      "while", "return", "+", "-", "*", "/", "=", "<", ">",
     *      "<=", ">=", "==", "!=", "+=", "-=", "*=", "/=",
     *      "(", ")", "{", "}", ";", ",", "<ID>", "<INT>"
     *  };
     */
    const std::vector<std::string> _terminals;
    static const char FLUSH = -1;
    void parsing_automata(char c = FLUSH);
    int get_terminal_id(const std::string& tokenVal);
    bool is_valid_terminal(const std::string&);
public:
    Lexer(std::vector<std::string>  terminals) : _terminals {std::move(terminals)} {};
    std::vector<Token> parse(const std::string& code);

    void parsing_automata();
};

#endif // LEXER_HPP
