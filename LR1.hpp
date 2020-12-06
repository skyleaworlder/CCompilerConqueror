#include <vector>
#include <string>
#include <map>
#include "baseGramma.hpp"

struct ActionDetail;
struct LR1Derivation;
struct ClosePkg;

using close_pkg_idx = int;

/**
 * @brief 操作集合
 * @param S: 移入
 * @param R: 规约
 * @param ACC: 接受
 * @param ERR: 错误
 */
enum Action {
    S,      // 移入
    R,      // 规约
    ACC,    // 接受
    ERR,    // 错误
};

/**
 * @brief 用于描述 Action 表项的结构体
 * @param action: 一个动作，可能是 移入 / 规约 / 接受 / 错误 这四种情况
 * @param toward: 动作搭配的数字，比如 S1(移入并到第1个), R2(规约并使用第2个)
 * @notes 如果是 ACC 或者 ERR，那么 toward 为 -1 吧
 */
struct ActionDetail {
    Action action;
    int toward;
};

/**
 * @brief 为了表示类似 [A -> a·Bb, b] 这样的项目
 * @param lr0_derivation_idx: 项目/推导式 在 LR0 derivations 中的 index
 * @param look_forward_symbol_idx: 前瞻符号 的 index
 */
struct LR1Derivation {
    // LR(0) 项目/推导式 在 LR0 derivations 中的 index
    int lr0_derivation_idx;
    // 前瞻符号 的 index
    int look_forward_symbol_idx;

    /**
     * @brief 重载判断相等，仅判断了 lr0_derivation_idx / look_forward_symbol_idx
     */
    bool operator == (const LR1Derivation& input) const {
        return (
            this->lr0_derivation_idx == input.lr0_derivation_idx
            && this->look_forward_symbol_idx == input.look_forward_symbol_idx
        );
    }
};

/**
 * @brief LR(1) 闭包,
 *      需要注意的是，闭包中除了项目集不存在任何 property 了
 * @param id: primary key
 * @param LR1_derivation_arr: LR(1) 项目集合
 */
struct ClosePkg {
    // primary key
    int id;
    /**
     * @brief 闭包中的 LR(1) 项目集合
     * @tparam LR1Derivation: LR1 的推导式类型
     */
    std::vector<LR1Derivation> LR1_derivation_arr;

    /**
     * @brief 重载判断相等，仅判断了 id / LR1_derivation_arr
     */
    bool operator == (const ClosePkg& input) const {
        return (
            this->id == input.id
            && this->LR1_derivation_arr == input.LR1_derivation_arr
        );
    }

    /**
     * @brief ClosePkg -- 判断闭包中是否存在某个 LR(1) 项目
     * @param input: 输入的 LR(1) 项目/推导式
     * @return boolean value
     */
    inline bool isExistLR1Drv(const LR1Derivation& input) {
        for (const auto& elem : this->LR1_derivation_arr) {
            if (elem == input)
                return true;
        }
        return false;
    }
};

class LR1Gramma : public BaseGrammar {
public:
    /**
     * @brief LR1Gramma -- LR(0) 的项目集
     * @tparam Derivation: LR(0) 推导式类型
     */
    std::vector<Derivation> lr0_derivations;
    /**
     * @brief LR1Gramma -- LR(1) 项目集族
     * @tparam ClosePkg: 闭包
     */
    std::vector<ClosePkg> C;

    /**
     * @brief LR1Gramma -- goto 表象的暂存区
     * @tparam close_pkg_idx: 闭包主键，唯一能够区分闭包的变量
     * @tparam symbol_idx: 符号主键，每个符号都有不同的变量
     */
    std::map<std::pair<close_pkg_idx, symbol_idx>, close_pkg_idx> goto_table_tmp;

    /**
     * @brief LR1Gramma -- goto 表象的暂存区
     * @tparam close_pkg_idx: 闭包主键，唯一能够区分闭包的变量
     * @tparam symbol_idx: 符号主键，每个符号都有不同的变量
     * @tparam ActionDetail: 用于描述 Action 表项的结构体
     */
    std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> action_table;
    /**
     * @brief LR1Gramma -- goto 表象的暂存区
     * @tparam close_pkg_idx: 闭包主键，唯一能够区分闭包的变量
     * @tparam symbol_idx: 符号主键，每个符号都有不同的变量
     * @tparam ActionDetail: 用于描述 Action 表项的结构体
     */
    std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> goto_table;

public:

    /**
     * @brief LR1Gramma -- 根据 dot_position / derive_idx / lr1_flag 等信息获取 id
     * @param derive_idx_inp: 输入 LR(0)/LR(1) 项目的源推导式 index
     * @param dot_position_inp: 输入项目的 点位置
     * @param lr1_flag_inp: 输入项目是否是 lr1 项目的标志位
     * @return derivation_idx 类型 表示 id
     */
    inline derivation_idx getLR0DrvIdByInfo(
        const derivation_idx derive_idx_inp,
        const int dot_position_inp,
        const bool lr1_flag_inp
    ) {
        for (const auto& LR0drv : this->lr0_derivations) {
            if (LR0drv.dot_position == dot_position_inp
                && LR0drv.derive_idx == derive_idx_inp
                && LR0drv.lr1_flag == lr1_flag_inp)
                return LR0drv.id;
        }
        // not found
        return -1;
    }

    /**
     * @brief LR1Gramma -- 获得 LR(0) 项目的 id 和 dot_position
     * @tparam lr0_idx: 表明要获得的 LR(0) 项目 id
     * @tparam int: 表示 dot_position
     * @param input: Derivation 类型 / LR(0)
     * @return first: id; second: dot_position
     */
    inline std::pair<lr0_idx, int> getLR0DrvIdByDrv(const Derivation& input) {
        for (const auto& LR0Drv : this->lr0_derivations) {
            if (input == LR0Drv)
                return { LR0Drv.id, LR0Drv.dot_position };
        }
        // not found
        return { -1, -1 };
    }

    /**
     * @brief LR1Gramma -- 获得 闭包 的 id，没有就返回 -1
     * @param input: ClosePkg
     * @return close_pkg_idx
     */
    inline close_pkg_idx isExistClosePkg(const ClosePkg& input) {
        for (const auto& elem : this->C) {
            if (elem == input)
                return elem.id;
        }
        return -1;
    }

    /**
     * @brief LR1Gramma -- 计算 LR(0) 项目
     */
    void calcuLR0Derivations();

    /**
     * @brief LR1Gramma -- 计算 ClosePkg 闭包
     * @param I: ClosePkg 类型 / 一般的输入一开始就是 LR(1) 项目集 I
     * @return ClosePkg 类型，输入参数 I 的闭包
     * @warning 需要格外注意，这个函数返回的是 修改过后的 I
     */
    ClosePkg calcuClosePkg(ClosePkg& I);

    /**
     * @brief LR1Gramma -- 给定一个 闭包 和 输入的符号，给出转移后的状态闭包
     * @param I: ClosePkg 类型
     * @param X: Symbol 类型。读入的字符肯定是 终结符 / 非终结符
     * @return j: ClosePkg 类型
     */
    ClosePkg GO(const ClosePkg& I, const Symbol& X);

    /**
     * @brief LR1Gramma -- 计算 LR(1) 项集族
     */
    void calcuLR1Derivations();

    /**
     * @brief LR1Gramma -- 计算 ACTION table
     */
    void calcuActionTable();

    /**
     * @brief LR1Gramma -- 计算 GOTO table
     */
    void calcuGotoTable();
};