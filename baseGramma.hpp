#ifndef BASE_GRAMMA_HPP
#define BASE_GRAMMA_HPP

#include "utils.hpp"
#include <set>

struct Symbol;
struct Derivation;

using Sym_Set = std::set<Symbol>;
using Sym_Arr = std::vector<Symbol>;
using Deri_Arr = std::vector<Derivation>;

using symbol_idx = int;
using derivation_idx = int;
// the same as derivation_idx
using lr0_idx = int;

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

    symbol_idx id;
    std::string name;
    Symbol::SYM_TYPE type;

    std::set<int> FIRST_SET;
    std::set<int> FOLLOW_SET;

    Symbol(
        const std::string name_inp,
        const Symbol::SYM_TYPE type_inp,
        const symbol_idx id_inp
    ) : name(name_inp), type(type_inp), id(id_inp) {}
    Symbol(): name(""), type(SYM_TYPE::END), id(-1) {}

    bool operator < (const Symbol& input) const {
        return id < input.id;
    }

    /**
     * @brief 仅仅判断了 id / name / type / FIRST / FOLLOW
     */
    bool operator == (const Symbol& input) const {
        return (
            this->id == input.id
            && this->name == input.name
            && this->type == input.type
            && this->FIRST_SET == input.FIRST_SET
            && this->FOLLOW_SET == input.FOLLOW_SET
        );
    }

    static bool isTerminal(const Symbol& input) {
        return input.type == SYM_TYPE::TERMINAL;
    }

    static bool isUnTerminal(const Symbol& input) {
        return input.type == SYM_TYPE::UNTERMINAL;
    }

    static bool isEnd(const Symbol& input) {
        return input.type == SYM_TYPE::END;
    }

    static bool isEpsilon(const Symbol& input) {
        return input.type == SYM_TYPE::EPSILON;
    }
};

/**
 * @brief LR(0) 项
 * @param id: primary key
 * @param left: 左边的符号
 * @param right: 右边的符号集
 * @param lr1_flag: 看看是否是 LR(1)
 * @param dot_postion: 如果是 LR(1) 才会有这个东西，表示项
 * @param derive_idx: 如果是 LR(0)，就是自己的编号，如果是 LR(1)，就是对应的 LR(0) 推导 (但是这个貌似没赋值，之后再捋捋)
 */
struct Derivation {

    Symbol left;
    // 为什么用 arr？
    // 因为不能用 set，用了 set 就得写 operator <
    // 这意味着 set 是会对插入的内容排序的
    // A -> aAb 会变成 A -> Aab (ascii)
    Sym_Arr right;

    bool lr1_flag;
    derivation_idx id;
    int dot_position;
    derivation_idx derive_idx;

    Derivation(
        Symbol left_inp, Sym_Arr right_inp,
        derivation_idx id_inp, bool lr1_flag_inp=false,
        int dot_position_inp=-1, derivation_idx derive_idx_inp=-1
    ) : left(left_inp), right(right_inp),
        id(id_inp), lr1_flag(lr1_flag_inp),
        dot_position(dot_position_inp), derive_idx(derive_idx_inp) {}

    bool operator < (const Derivation &input) const {
        return id < input.id;
    }

    /**
     * @brief Derivation -- 重载判断相等,
     *      仅仅判断了 id / dot_position / derive_idx /
     *                  lr1_flag / left / right
     */
    bool operator == (const Derivation &second) const {
        return (
            this->id == second.id
            && this->dot_position == second.dot_position
            && this->derive_idx == second.derive_idx
            && this->lr1_flag == second.lr1_flag
            && this->left == second.left
            && this->right == second.right
        );
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

    // BaseGramma -- 符号集
    Sym_Arr symbol_arr;
    /**
     * @brief BaseGramma -- 终结符集
     *        wqnm，set 到底是怎么判定相同节点的？wdnmd
     */
    std::set<int> terminal_set;
    // BaseGramma -- 非终结符集
    std::set<int> unterminal_set;

    /**
     * @brief BaseGramma -- 推导式集
     *        所以为什么这个用 arr，就是不知道为什么 set insert 之后 size 永远是 1
     *        wqnmd
     */
    Deri_Arr derivation_set;

    // S -> S' 这个推导式的 id
    derivation_idx init_derivation;

public:
    /**
     * @brief BaseGramma -- 文法读取
     * @param file_path: 输入的文法路径
     */
    void readGramma(const std::string file_path);
	//将second集合中的元素加入到first集合，若with_epsilon=true，则连通epsilon一起合并，否则去掉epsilon
	std::pair<bool, std::set<int>> mergeSet(const std::set<int> first, const std::set<int> second, bool with_epsilon);
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
    inline std::pair<int, Symbol::SYM_TYPE> getSymIdByName(const std::string name) {
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

    inline Symbol getSymByName(const std::string name) {
        for (const auto& elem : this->symbol_arr)
            if (elem.name == name)
                return elem;
		Symbol ch("#", Symbol::SYM_TYPE::END, -1);
        return ch;
    }
};

#endif