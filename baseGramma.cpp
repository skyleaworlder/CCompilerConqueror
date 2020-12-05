#include "baseGramma.hpp"

const std::string BaseGrammar::Origin = "S";
const std::string BaseGrammar::OriginPro = "Program";
const std::string BaseGrammar::Derive = "->";
const std::string BaseGrammar::Comment = "~";
const std::string BaseGrammar::EndSym = "#";
const std::string BaseGrammar::Epsilon = "$";
const std::string BaseGrammar::Alternative = "|";
const std::string BaseGrammar::TerminalLineBeg = "!";

/**
 * @brief 读取文法内容，填入 terminal/unterminal/symbol/derivation
 * @param file_path: 文法文件的位置
 * @see http://www.cplusplus.com/reference/string/string/getline/
 */
void BaseGrammar::readGramma(std::string file_path) {
    std::ifstream in(file_path);
    std::string line_read;

    Symbol end(EndSym, Symbol::END, 0);
    Symbol ep(Epsilon, Symbol::EPSILON, 1);

    // 初始化
    this->symbol_arr.emplace_back(end);
    this->terminal_set.insert(0);
    this->symbol_arr.emplace_back(ep);

    // 开始读取
    // std::getline from <string>
    while(std::getline(in, line_read, '\n')) {
        // 是注释 / 空行
        if (line_read[0] == '~' || line_read.length() == 0)
            continue;

        // 这里人为保证一行只有一个推导式
        std::vector<std::string> split_res;
        split_res = split(line_read, " -> ");
        std::for_each(split_res.begin(), split_res.end(), trim);
        // 左右分开
        std::string left_str = trim(split_res[0]);
        std::string right_str = split_res[1];

        // 接下来对右侧进行处理：A -> a B b | A C | D d
        std::vector<std::string> right_symbols_multi_derives;
        right_symbols_multi_derives = split(right_str, " | ");

        // 取每一个推导，做处理
        for (auto right_symbols : right_symbols_multi_derives) {
            // 首先处理所有的非终结符
            if (line_read[0] == '!') {
                Symbol terminal(right_symbols, Symbol::TERMINAL, this->symbol_arr.size());
                // 顺序不能颠倒，因为 getSymIdByName 是在 symbol_arr 里面去找的
                this->symbol_arr.emplace_back(terminal);
                this->terminal_set.insert(getSymIdByName(right_symbols).first);
            }
            else {
                std::vector<std::string> right_symbol_arr = split(right_symbols, " ");
                std::for_each(right_symbol_arr.begin(), right_symbol_arr.end(), trim);

                // 新增未出现过的 非终结符
                // 先看左边
                auto get_res = getSymIdByName(left_str);
                Symbol left = (-1 == get_res.first)
                    ? (Symbol(left_str, Symbol::UNTERMINAL, this->symbol_arr.size()))
                    : (Symbol(left_str, get_res.second, get_res.first));
                if (-1 == get_res.first) {
                    this->symbol_arr.emplace_back(left);
                    // 因为在 symbol arr 中添加过了，所以可以找到，并且完成添加
                    this->unterminal_set.insert(getSymIdByName(left_str).first);
                }
                // 对于右边
                Sym_Arr right;
                for (const std::string& elem : right_symbol_arr) {
                    if (-1 == getSymIdByName(elem).first) {
                        Symbol sym_new(elem, Symbol::UNTERMINAL, this->symbol_arr.size());
                        this->symbol_arr.emplace_back(sym_new);
                        this->unterminal_set.insert(getSymIdByName(elem).first);
                        right.emplace_back(sym_new);
                    }
                    else
                        right.emplace_back(getSymByName(elem));
                }

                // convert type std::vector to std::set
                Derivation new_derive(left, right, this->derivation_set.size());
                this->derivation_set.emplace_back(new_derive);
            }
        }
    }
}