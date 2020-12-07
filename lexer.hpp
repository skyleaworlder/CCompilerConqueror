#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include <string>
#include <set>
#include <list>

const std::set<std::string> Terminal = {
    "int", "float", "double", "void", "if", "else",
    "while", "return", "+", "-", "*", "/", "=", "<", ">",
    "<=", ">=", "==", "!=", "+=", "-=", "*=", "/=",
    "(", ")", "{", "}", ";", ",", "<ID>", "<INT>"
};

/**
 * @brief 词法分析器 token.
 *          比如 { 0, 10, "int", "int" }, { 30, 1, "<INT>", "45" }
 * @param id: 唯一标识符, 根据 Terminal 中的排序得到
 * @param line_number: 行号，为后续开发做准备
 * @param name: 当前 token 在 Terminal 集中的值
 * @param value: 真正在输入程序中的表示
 */
struct token {
    int id;
    int line_number;
    std::string name;
    std::string value;
};

/**
 * @brief 词法分析器 Lexer
 * @param token_list: list<token> 类型，词法分析结束后的 token 存放位置
 */
class Lexer {

public:
    std::list<token> token_list;

public:
    void LexParse(std::string file_path);

};

#endif