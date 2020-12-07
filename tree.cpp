#include "tree.hpp"
#include <stack>
#include <algorithm>
using namespace std;

//树的构造函数 传入词法输出 产生式 action_table goto_table
tree::tree(std::list<token> token_list, std::vector<Derivation> deriv,std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> action_table, std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> goto_table)
{
    bool acc = false;
    std::stack<TreeNode> symStack;
    std::stack<close_pkg_idx> stateStack;
    TreeNode* tmp, * now_node;
    tmp = new(nothrow)TreeNode;
    tmp->id = 0;
    tmp->name = "#";
    symStack.push(*tmp);//初始栈底为#
    stateStack.push(0);//初始压入状态0

    //遍历词法分析的结果
    for (list<token>::iterator iter = token_list.begin(); iter != token_list.end(); ) {

        /* debug
        std::cout << "symStack size: " << symStack.size() << std::endl;
        std::cout << "state Stack size: " << stateStack.size() << std::endl;
        */

        //iter指向词法分析的当前的单词
        symbol_idx now_id = iter->id;
        string now_name = iter->name;
        close_pkg_idx top_state = stateStack.top();//当前状态栈顶的值

        map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail>::iterator it;
        it=action_table.find(pair<close_pkg_idx, symbol_idx>(top_state, now_id));
        //action表中没有对应的状态 错误
        if (it == action_table.end()) {
            tree_root.record = -1;
            break;
        }
        //有对应的状态
        else {
            ActionDetail now_act = action_table[{top_state, now_id}];//获取到当前需要进行的动作

            //语法分析成功
            if (now_act.action == ACC) {
                tree_root.record = 2;
                //根指向栈顶pop出的节点
                TreeNode* last = new(nothrow) TreeNode;
                *last = symStack.top();
                //将头指向初始的符号
                tree_root.child.push_back(last);
                //std::cout << tree_root.child.size() << std::endl;
                acc = true;
                break;
            }

            //移入 一定是终结符
            else if (now_act.action == S) {
                now_node = new(nothrow)TreeNode;//当前读入的单词生成一个树节点
                now_node->id = iter->id;
                now_node->name = iter->name;
                now_node->record = 1;//终结符
                symStack.push(*now_node);//压入符号的值
                stateStack.push(now_act.toward);//压入当前的状态
                iter++;//词法结果后移
            }

            //规约
            else if (now_act.action == R) {
                int aim_driv = now_act.toward;//当前规约的产生式
                TreeNode* father = new(nothrow)TreeNode;
                father->name = deriv[aim_driv].left.name;
                father->id = deriv[aim_driv].left.id;
                father->record = 0;//非终结符
                int length = deriv[aim_driv].right.size();//得到右边有几个要规约的符号
                //出栈过程
                for (int i = 0; i < length; i++) {
                    TreeNode* child = new(nothrow)TreeNode;
                    *child = symStack.top();//获取当前符号栈顶符号
                    father->child.push_back(child);//将当前符号找到父节点
                    symStack.pop();//出栈
                    stateStack.pop();//符号栈出一个，状态栈对应也要出栈一个
                }

                //将孩子顺序逆转
                std::reverse(father->child.begin(), father->child.end());

                //父节点入栈
                symStack.push(*father);

                //goto表
                //检查是否有状态
                map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail>::iterator it_go;
                it_go = goto_table.find(pair<close_pkg_idx, symbol_idx>(stateStack.top(), symStack.top().id));
                //分析出错
                if (it_go == goto_table.end()) {
                    tree_root.record = -1;
                    break;
                }
                else {
                    //有 获取goto状态
                    ActionDetail now_goto = goto_table[{stateStack.top(), symStack.top().id}];//获取到当前需要转移的状态
                    if (now_goto.action == R&&now_goto.toward!=-1) {
                        //将当前转移状态压栈
                        stateStack.push(now_goto.toward);
                    }
                    else {
                        tree_root.record = -1;
                        break;
                    }
                }
            }

            //语法分析错误
            else {
                tree_root.record = -1;
                break;
            }
        }
    }
    if (!acc) {
        tree_root.record = -1;
    }

}