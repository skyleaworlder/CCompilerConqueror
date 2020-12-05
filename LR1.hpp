#include <vector>
#include <string>
#include <map>
#include "baseGramma.hpp"

enum Action;
struct ActionDetail;
struct LR1Derivation;
struct ClosePkg;

using close_pkg_idx = int;

class LR1Gramma : BaseGrammar {
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
    LR1Gramma(/* args */);
    ~LR1Gramma();

    /**
     * @brief LR1Gramma -- 计算 LR(0) 项目
     */
    void calcuLR0Derivations();

    /**
     * @brief LR1Gramma -- 获得 LR(0) 项目的 id 和 dot_position
     * @tparam lr0_idx: 表明要获得的 LR(0) 项目 id
     * @tparam int: 表示 dot_position
     */
    inline std::pair<lr0_idx, int> getLR0DrvIdByDrv(const Derivation& input) {

    }
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