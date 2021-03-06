#ifndef tree_HPP
#define tree_HPP
#include <string>
#include <vector>
#include <list>
#include "baseGramma.hpp"
#include "lexer.hpp"
#include <map>
#include "LR1.hpp"
#include <stack>

//树节点
struct TreeNode {
    symbol_idx id;//机内码
    std::string name;//名字
    int record;//1为终结符，0为非终结符,-1为语法错误,2为成功生成了树
    std::vector<TreeNode*> child;
};

struct tree_list {
    std::string node_name;
    int index;
    std::vector<int> child;//存放孩子
};

class tree {
public:
    TreeNode tree_root;
    std::vector<tree_list> Tree;

public:
    tree(std::list<Token> token_list,
        std::vector<Derivation> deriv,
        std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> action_table,
        std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> goto_table
    );
    void tree_LevelTraverse();
    void print_tree();
    void output_parsing_analyze(
        int count_step, std::ofstream& out,
        std::stack<TreeNode> symStack,
        std::stack<close_pkg_idx> stateStack
    );
};


#endif