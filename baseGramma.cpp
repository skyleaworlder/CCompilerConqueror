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
void BaseGrammar::readGramma(const std::string file_path) {
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


std::pair<bool, std::set<int>> BaseGrammar::mergeSet(const std::set<int> first, const std::set<int> second, bool with_epsilon)
{
	bool success = false;
	if (first == second)
		return{ success,first };

	auto res = getSymIdByName(Epsilon);
	std::set<int> temp_first, temp_second;
	temp_first.insert(first.begin(), first.end());
	temp_second.insert(second.begin(), second.end());
	if (!with_epsilon)
		temp_second.erase(res.first);
	temp_first.insert(temp_second.begin(), temp_second.end());
	if (first.size() < temp_first.size())
		success = true;
	return{ success,temp_first };
}


void BaseGrammar::calcuSingleUnterminalFirstSet(int unterminal)
{
	for (auto derivation : this->derivation_set)
	{
		if (derivation.left.name == this->symbol_arr[unterminal].name)
		{
			auto res_left_letter = getSymIdByName(derivation.left.name);
			//产生式右边第一个字符为终结符或epsilon时，直接将该字符加入，然后继续查找下一个推导式
			if (derivation.right[0].type == Symbol::TERMINAL || derivation.right[0].type == Symbol::EPSILON)
			{
				this->symbol_arr[res_left_letter.first].FIRST_SET.insert(derivation.right[0].id);
				continue;
			}
			//产生式右边为非终结符
			else
			{
				int count = 0;//记录共存入count个非终结符的first集
				for (auto single_right_letter : derivation.right)
				{
					auto res_right_letter = getSymIdByName(single_right_letter.name);
					if (this->symbol_arr[res_right_letter.first].FIRST_SET.size() == 0)
						calcuSingleUnterminalFirstSet(res_right_letter.first);
					if (single_right_letter.type == Symbol::TERMINAL)
					{
						this->symbol_arr[res_left_letter.first].FIRST_SET.insert(single_right_letter.id);
						break;
					}
					if (this->symbol_arr[res_right_letter.first].FIRST_SET.count(getSymByName(Epsilon).id))
					{
						auto ret = this->mergeSet(
							this->symbol_arr[res_left_letter.first].FIRST_SET,
							this->symbol_arr[res_right_letter.first].FIRST_SET,
							false
						);
						this->symbol_arr[res_left_letter.first].FIRST_SET = ret.second;
						count++;
					}
					else {
						auto ret = this->mergeSet(
							this->symbol_arr[res_left_letter.first].FIRST_SET,
							this->symbol_arr[res_right_letter.first].FIRST_SET,
							false
						);
						this->symbol_arr[res_left_letter.first].FIRST_SET = ret.second;
						break;
					}
					if (count == derivation.right.size())
					{
						auto res_Epsilon = getSymIdByName(Epsilon);
						this->symbol_arr[res_left_letter.first].FIRST_SET.insert(res_Epsilon.first);
					}
				}
			}
		}
	}
}

void BaseGrammar::calcuAllUnterminalFirstSet()
{
	for (auto unterminal_index : this->unterminal_set)
		calcuSingleUnterminalFirstSet(unterminal_index);
}

void BaseGrammar::calcuAllTerminalFirstSet()
{
	for (auto terminal_index : this->terminal_set)
		this->symbol_arr[terminal_index].FIRST_SET.insert(terminal_index);
}

void BaseGrammar::calcuSingleUnterminalFollowSet(int unterminal)
{
	for (auto derivation : this->derivation_set)
	{
		for (auto single_right_letter : derivation.right)
		{
			int next_symbol_index = 0;
			auto res_right_letter = getSymIdByName(single_right_letter.name);
			//说明在某个推导式的右边找到了该非终结符
			if (res_right_letter.first == unterminal)
			{
				//说明该非终结符后面没有符号
				if (next_symbol_index == derivation.right.size())
				{
					/* TODO */
				}
				//取出该非终结符后面的符号
				auto next_symbol = derivation.right[next_symbol_index + 1];
			}
			else
				next_symbol_index++;
		}
	}
}
void BaseGrammar::calcuAllUnterminalFollowSet()
{
	for (auto terminal_index : this->terminal_set)
		calcuSingleUnterminalFollowSet(terminal_index);
}