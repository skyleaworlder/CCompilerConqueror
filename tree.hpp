#ifndef tree_HPP
#define tree_HPP
#include <string>
#include <vector>
#include <list>
#include "baseGramma.hpp"
#include "lexer.hpp"
#include <map>
#include "LR1.hpp"

//树节点
struct TreeNode {
    symbol_idx id;//机内码
    std::string name;//名字
    int record;//1为终结符，0为非终结符,-1为语法错误,2为成功生成了树
    std::vector<TreeNode*> child;
};

class tree {
public:
    TreeNode tree_root;
public:
    tree(
        std::list<token> token_list,
        std::vector<Derivation> deriv,
        std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> action_table,
        std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> goto_table
    );
};
#endif