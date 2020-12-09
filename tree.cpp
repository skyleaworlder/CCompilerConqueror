#include "tree.hpp"
#include <stack>
#include<queue>
#include <algorithm>
using namespace std;

//树的构造函数 传入词法输出 产生式 action_table goto_table
tree::tree(std::list<Token> token_list, std::vector<Derivation> deriv,std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> action_table, std::map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail> goto_table)
{
    bool acc = false;
    std::ofstream out_analyze;
    out_analyze.open("parsing_analyze.txt");
    int count_step = 0;
    std::stack<TreeNode> symStack;
    std::stack<close_pkg_idx> stateStack;
    TreeNode* tmp, * now_node;
    tmp = new(nothrow)TreeNode;
    tmp->id = 0;
    tmp->name = "#";
    symStack.push(*tmp);//初始栈底为#
    stateStack.push(0);//初始压入状态0
    output_parsing_analyze(count_step, out_analyze, symStack, stateStack);
    out_analyze << "动作:" << "初始状态" << endl<<endl;
    //遍历词法分析的结果
    for (list<Token>::iterator iter = token_list.begin(); iter != token_list.end(); ) {

        //iter指向词法分析的当前的单词
        symbol_idx now_id = iter->id;
        string now_name = iter->name;
        close_pkg_idx top_state = stateStack.top();//当前状态栈顶的值

        map<std::pair<close_pkg_idx, symbol_idx>, ActionDetail>::iterator it;
        it=action_table.find(pair<close_pkg_idx, symbol_idx>(top_state, now_id));
        //action表中没有对应的状态 错误
        if (it == action_table.end()) {
            tree_root.record = -1;
            out_analyze << "action表中未找到对应动作，语法分析失败" << endl << endl;
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
                acc = true;
                count_step++;
                output_parsing_analyze(count_step, out_analyze, symStack, stateStack);
                out_analyze << "动作：" << "成功" << endl << endl;
                cout << "语法分析成功" << endl;
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
                count_step++;
                output_parsing_analyze(count_step, out_analyze, symStack, stateStack);
                out_analyze << "动作：" << "移进" << endl << endl;

            }

            //规约
            else if (now_act.action == R) {
                int aim_driv = now_act.toward;//当前规约的产生式
                TreeNode* father = new(nothrow)TreeNode;
                father->name = deriv[aim_driv].left.name;
                father->id = deriv[aim_driv].left.id;
                father->record = 0;//非终结符

                int length;
                //判断是不是$ 是的话入栈 不pop
                if (deriv[aim_driv].right[0].name == "$") {//得到右边有几个要规约的符号
                    length = 0;
                }
                else
                    length = deriv[aim_driv].right.size();

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
                    out_analyze << "goto表中无响应状态转移，语法分析失败" << endl << endl;
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
                        out_analyze << "语法分析失败" << endl << endl;
                        break;
                    }
                }
                count_step++;
                output_parsing_analyze(count_step, out_analyze, symStack, stateStack);
                out_analyze << "动作：" << "规约" << endl<<endl;
            }

            //语法分析错误
            else {
                tree_root.record = -1;
                out_analyze << "语法分析失败" << endl << endl;
                break;
            }
        }
    }
    if (!acc) {
        tree_root.record = -1;
        cout << "语法分析失败" << endl;
    }

}

//层次遍历存放树节点
void tree::tree_LevelTraverse() {
    std::queue<TreeNode> LT_queue;//用于层次遍历的存放节点的队列
    tree_list* cur_node;//生成list的格式存入list中
    TreeNode head;//从queue中取出的头
    int child_length, tree_length;//每个节点对应的孩子的长度 已经存放入list的长度

    //队列先放入头结点
    LT_queue.push(*tree_root.child[0]);//先将根节点入队列
    head = LT_queue.front();//队列中的头

    //先将头放入Tree中
    cur_node = new(nothrow)tree_list;
    cur_node->node_name = head.name;
    cur_node->index = 0;
    Tree.push_back(*cur_node);

    for (int i = 0; !LT_queue.empty();i++) {
        tree_length = this->Tree.size();//已存入Tree中的节点个数
        head = LT_queue.front();//获取队列中的头
        //存孩子
        child_length = head.child.size();
        for (int j = 0; j < child_length; j++) {
            cur_node = new(nothrow)tree_list;
            cur_node->node_name = head.child[j]->name;
            cur_node->index = tree_length + j;
            /*if (head.record == 1)
                cur_node->child.push_back(-1);*/
            Tree[i].child.push_back(cur_node->index);//对树中的节点标记孩子
            this->Tree.push_back(*cur_node);//将节点存入树中
            LT_queue.push(*head.child[j]);//将孩子放入队列中
        }
        LT_queue.pop();
    }

}

void tree::print_tree() {
    bool bo = false;
    std::ofstream out;
    out.open("./data/tree.txt");

    if (!out)
        cout << "Fail open file!" << endl;
    if (this->tree_root.record == -1) {
        out << "error" << endl;
    }
    else {

        this->tree_LevelTraverse();
        out << Tree[0].node_name<<endl;

        for (int i = 0; i < this->Tree.size(); i++) {
            bo = false;
            if (this->Tree[i].child.size()) {
                out << this->Tree[i].index << "->";
                for (int j = 0; j < this->Tree[i].child.size(); j++) {
                    if (!bo) {
                        out << this->Tree[i].child[j]<<":"<<this->Tree[this->Tree[i].child[j]].node_name;
                        bo = true;
                    }
                    else 
                        out <<" "<< this->Tree[i].child[j] << ":" << this->Tree[this->Tree[i].child[j]].node_name;

                }
                out << endl;
            }

        }
    }
}

void tree::output_parsing_analyze(int count_step,std::ofstream& out, std::stack<TreeNode> symStack,std::stack<close_pkg_idx> stateStack)
{
    out << "步骤：" << count_step << endl;
    stack<int>temp_state = stateStack;
    stack<int>other_state;
    while (!temp_state.empty()) {
        other_state.push(temp_state.top());
        temp_state.pop();
    }
    out << "状态栈：";
    while (!other_state.empty()) {
        out << other_state.top() << " ";
        other_state.pop();
    }
    out << endl ;

    stack<TreeNode>temp_sym = symStack;
    stack<TreeNode>other_sym;
    while (!temp_sym.empty()) {
        other_sym.push(temp_sym.top());
        temp_sym.pop();
    }
    out << "符号栈：";
    while (!other_sym.empty()) {
        out << other_sym.top().name << " ";
        other_sym.pop();
    }
    out << endl;

}
