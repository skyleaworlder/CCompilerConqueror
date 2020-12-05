#ifndef SYNTAX_PARSER_HPP
#define STNTAX_PARSER_HPP

#include "utils.hpp"
#include <unordered_set>

struct Symbol;
struct Derivation;

using Sym_Set = std::unordered_set<Symbol>;
using Sym_Arr = std::vector<Symbol>;
using Deri_Arr = std::vector<Derivation>;

/**
 * @brief 符号 结构体
 * @param id: primary key
 * @param name: 这个符号在文法中的形式，如 "$", "#", "GlbDefList", "BRACE"
 * @param type: 这个符号的类型
 * @param FIRST_SET
 * @param FOLLOW_SET
 */
struct Symbol {

    enum SYM_TYPE {
        TERMINAL,
        UNTERMINAL,
        END,
        EPSILON
    };

    size_t id;
    std::string name;
    Symbol::SYM_TYPE type;

    std::unordered_set<int> FIRST_SET;
    std::unordered_set<int> FOLLOW_SET;

    Symbol(
        const std::string name_inp,
        const Symbol::SYM_TYPE type_inp,
        const size_t id_inp
    ) : name(name_inp), type(type_inp), id(id_inp) {}

    bool operator < (const Symbol& input) const {
        return id < input.id;
    }
};

/**
 * @brief LR(0) 项
 * @param id: primary key
 * @param left: 左边的符号
 * @param right: 右边的符号集
 * @param lr1_flag: 看看是否是 LR(1)
 * @param dot_postion: 如果是 LR(1) 才会有这个东西，表示项
 * @param derive_idx: 如果是 LR(0)，就是自己的编号，如果是 LR(1)，就是对应的 LR(0) 推导
 */
struct Derivation {

    Symbol left;
    // 为什么用 arr？
    // 因为不能用 set，用了 set 就得写 operator <
    // 这意味着 set 是会对插入的内容排序的
    // A -> aAb 会变成 A -> Aab (ascii)
    Sym_Arr right;

    bool lr1_flag;
    int id;
    int dot_position;
    int derive_idx;

    Derivation(
        Symbol left_inp, Sym_Arr right_inp,
        bool lr1_flag_inp=false,
        int dot_position_inp=-1, int derive_idx_inp=-1
    ) : left(left_inp), right(right_inp),
        lr1_flag(lr1_flag_inp),
        dot_position(dot_position_inp), derive_idx(derive_idx_inp) {}

    bool operator < (const Derivation &input) const {
        return id < input.id;
    }
};

/**
 * @brief 基本文法功能
 * @param symbol_arr: 符号集
 * @param terminal_set: 终结符集合
 * @param unterminal_set: 非终结符集合
 * @param derivation_set: 推导式集合
 */
class BaseGrammar {
public:

    static const std::string Origin;
    static const std::string Derive;
    static const std::string OriginPro;
    static const std::string Alternative;
    static const std::string Epsilon;
    static const std::string Comment;
    static const std::string TerminalLineBeg;
    static const std::string EndSym;

    Sym_Arr symbol_arr;
    // wqnm，set 到底是怎么判定相同节点的？wdnmd
    std::unordered_set<int> terminal_set;
    std::unordered_set<int> unterminal_set;

    // 所以为什么这个用 arr，就是不知道为什么 set insert 之后 size 永远是 1
    // wqnmd
    Deri_Arr derivation_set;

public:
    void readGramma(std::string file_path);
	//将second集合中的元素加入到first集合，若with_epsilon=true，则连通epsilon一起合并，否则去掉epsilon
	std::pair<bool, std::unordered_set<int>> mergeSet(const std::unordered_set<int> first, const std::unordered_set<int> second, bool with_epsilon);
	void calcuSingleUnterminalFirstSet(int unterminal);
	void calcuAllUnterminalFirstSet();
	void calcuAllTerminalFirstSet();
	void calcuSingleUnterminalFollowSet(int unternimal);
	void calcuAllUnterminalFollowSet();
	

public:

    /**
     * @brief 根据 name 获取 id，其实也可以当成检验符号是否存在的函数
     * @param name
     * @return 如果存在就返回 id 和 type，如果没有就返回 -1
     */
    inline std::pair<int, Symbol::SYM_TYPE> getSymIdByName(std::string name) {
        Sym_Arr::iterator iter = std::find_if(
            this->symbol_arr.begin(),
            this->symbol_arr.end(),
            [&](Symbol sym_in_vec) { return sym_in_vec.name == name; }
        );

        // if failed, iter will be end()
        if (iter != this->symbol_arr.end())
            return { (*iter).id, (*iter).type };
        return { -1, Symbol::SYM_TYPE::END };
    }

    inline Symbol getSymByName(std::string name) {
        for (auto& elem : this->symbol_arr)
            if (elem.name == name)
                return elem;
		Symbol ch("#", Symbol::SYM_TYPE::END, -1);
        return ch;
    }
};

#endif