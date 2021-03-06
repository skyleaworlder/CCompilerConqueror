#include "LR1.hpp"

void LR1Gramma::calcuLR0Derivations() {
    // S -> Program
    // => S -> ·Program ; S -> Program·
    for (size_t index = 0; index < this->derivation_set.size(); ++index) {
        for (size_t dot_pos = 0; dot_pos <= this->derivation_set[index].right.size(); ++dot_pos) {
            this->lr0_derivations.emplace_back(this->derivation_set[index]);
            // back point to the last position
            // while end point point to the next pos of back()
            this->lr0_derivations.back().id = this->lr0_derivations.size()-1;
            this->lr0_derivations.back().dot_position = dot_pos;
            this->lr0_derivations.back().lr1_flag = true;
            // derive_idx equal -1 when construct Derivation
            // while id is AUTO_INCREMENT
            this->lr0_derivations.back().derive_idx = this->derivation_set[index].id;
        }
    }
	//std::cout << std::endl;
}

// TODO: 死循环: lr0_derivation_idx 为空
ClosePkg LR1Gramma::calcuClosePkg(ClosePkg& I) {
    // 遍历 I 中的每一个 LR(1) 项目
    for (size_t index = 0; index < I.LR1_derivation_arr.size(); ++index) {
        // 取出的 LR(1) 项目 [A -> a·Bb, a]
        const LR1Derivation lr1_drv = I.LR1_derivation_arr[index];
        //std::cout << "heheh" << std::endl;
        //std::cout << lr1_drv.lr0_derivation_idx << std::endl;
        // 取出的 LR(0) 项目 [A -> a.Bb]
        const Derivation lr0_drv = this->lr0_derivations[lr1_drv.lr0_derivation_idx];
        //std::cout << "hohoho" << std::endl;

        // 不存在 b, 只是 [A -> aB·] (dot_position = 2; size = 2)
        // 因为后面还要取 FIRST(ba)，所以必须小于
        if (lr0_drv.dot_position >= lr0_drv.right.size())
            continue;

        // [A -> a·Bb], dot_position = 1, arr[1] = B
        const Symbol B = lr0_drv.right[lr0_drv.dot_position];
        // 必须为非终结符
        if (Symbol::isTerminal(B))
            continue;
        // 因为文法不可能出现 A -> B $
        // 因此，只要 B 是 $，那么必有 A -> ·$
        // 将 A -> ·$ => A -> $·
        if (Symbol::isEpsilon(B)) {
            Derivation lr0_drv_mod_dot = lr0_drv;
            lr0_drv_mod_dot.dot_position += 1;
            lr0_drv_mod_dot.id = getLR0DrvIdByInfo(
                lr0_drv_mod_dot.derive_idx,
                lr0_drv_mod_dot.dot_position,
                lr0_drv_mod_dot.lr1_flag
            );
            // 必定存在的前提是，LR(0) 项目生成正常，链接到 A -> $· 项目
            I.LR1_derivation_arr[index].lr0_derivation_idx = this->getLR0DrvIdByDrv(lr0_drv_mod_dot).first;
            continue;
        }

        // 构造是左闭右开
        // 对于 [A -> alpha ·B beta, a]，这里首先取到的是 beta
        std::vector<int> beta_a;
        std::vector<Symbol> beta_a_sym(
            lr0_drv.right.begin()+lr0_drv.dot_position+1,
            lr0_drv.right.end()
        );
        for (const auto& elem : beta_a_sym)
            beta_a.emplace_back(elem.id);
        beta_a.emplace_back(lr1_drv.look_forward_symbol_idx);

        std::set<int> beta_a_first_set = calcuSymbolStringFirstSet(beta_a);

        // 对于每个 B -> ·gama 产生式，都取出来分析一下
        // 首先，需要判断左侧是否为 B
        // 其次，要考虑点是否在最左侧
        // 再次，右侧一般为 终结符 / 非终结符
        //      如果是 epsilon，添加的是 B -> $·
        for (const auto& drv : this->lr0_derivations) {
            // 如果左边不是 B，那就下一个
            // 我只写了 ==，没写 !=
            // std::cout << drv.left.name << std::endl;
            if (!(drv.left == B))
                continue;
            else {
                // begin 和 front 位置相同
                // 正常符号但不在最开始，那肯定过滤掉
                if (!Symbol::isEpsilon(drv.right.front()) && drv.dot_position != 0)
                    continue;
                // gama 是 epsilon，但 dot 在开头
                if (Symbol::isEpsilon(drv.right.front()) && drv.dot_position == 0)
                    continue;
            }
            // 对于 FIRST(beta a) 中的每个终结符 b
            for (const auto& b : beta_a_first_set) {
                if (!Symbol::isEpsilon(this->symbol_arr[b]) && !I.isExistLR1Drv({ drv.id, b }))
                    I.LR1_derivation_arr.push_back({ drv.id, b });
            }
        }
    }
    return I;
}

ClosePkg LR1Gramma::GO(const ClosePkg& I, const Symbol& X) {
    ClosePkg j;
    j.id = this->C.size();
    if (Symbol::isEnd(X) || Symbol::isEpsilon(X))
        return j;

    // 不能用 范围 for 循环
    for (size_t lr1_drv_idx = 0; lr1_drv_idx < I.LR1_derivation_arr.size(); lr1_drv_idx++) {
        // 找到所有的 [A -> alpha ·X beta, a] \in I
        const auto lr1_drv = I.LR1_derivation_arr[lr1_drv_idx];
        const auto lr0_drv = this->lr0_derivations[lr1_drv.lr0_derivation_idx];
        //std::cout << "lr0 drv: id, name: " << lr0_drv.id << lr0_drv.left.name << std::endl;
        //std::cout << "lr1 derive_id: " << lr1_drv.lr0_derivation_idx <<std::endl;

		// 排除 [A -> alpha·]
		if (lr0_drv.dot_position >= lr0_drv.right.size())
			continue;
        //std::cout << "except1" <<std::endl;
        // 排除 [A -> alpha ·Y beta, a] (点后非 A)
        // 我没实现 !=
        //std::cout << "X: (" << X.id << ", " << X.name << ")" << std::endl;
		if (!(X == lr0_drv.right[lr0_drv.dot_position]))
			continue;
        //std::cout << "except2" <<std::endl;

        Derivation lr0_drv_dot_back = lr0_drv;
        lr0_drv_dot_back.dot_position += 1;
        lr0_drv_dot_back.id = getLR0DrvIdByInfo(
            lr0_drv_dot_back.derive_idx,
            lr0_drv_dot_back.dot_position,
            lr0_drv_dot_back.lr1_flag
        );
        //std::cout << "lr0 dot back id: " << lr0_drv_dot_back.id << std::endl;
        // TODO: 这里可能有问题，因为 C 的 size 没有变
        // 很可能存在一堆相同 id 的 C
        j.LR1_derivation_arr.push_back({
            getLR0DrvIdByDrv(lr0_drv_dot_back).first,
            lr1_drv.look_forward_symbol_idx
        });

        /* debug
        using std::cout;
        using std::endl;
        cout << "ClosePkg: " << this->C.size() << endl;
        for (const auto& I : this->C) {
            cout << I.id << ":" << endl;
            for (auto drv : I.LR1_derivation_arr) {
                cout << "(derivation, " << drv.lr0_derivation_idx << ") "
                    << "(lf_sym, " << drv.look_forward_symbol_idx << ")" << endl;
            }
            cout << endl << endl;
        }
        */
    }
    return calcuClosePkg(j);
}


void LR1Gramma::calcuLR1Derivations() {
    // 将 [S -> ·Program, #] 加入到 LR(1) 项目集中
    /*
    Derivation S_to_Program;
    for (const auto& elem : this->derivation_set)
        if (elem.left.name == "S") {
            S_to_Program = elem;
            break;
        }
    */
    Derivation S_to_Program = *(std::find_if(this->lr0_derivations.begin(), this->lr0_derivations.end(), [&](Derivation elem) {
        return (elem.left.name == "S");
    }));
    ClosePkg init_closepkg;
    // 初始化第一个闭包
    init_closepkg.id = 0;
    /* debug
    std::cout << "S -> Program: " << S_to_Program.id << " " << S_to_Program.left.name << " " << S_to_Program.right.size() << std::endl;
    std::cout << "              " << S_to_Program.dot_position << " " << S_to_Program.lr1_flag << std::endl;
    std::cout << "drv id: " << getLR0DrvIdByDrv(S_to_Program).first << " " << getSymIdByName(EndSym).first << std::endl;
    */
    init_closepkg.LR1_derivation_arr.push_back({
        getLR0DrvIdByDrv(S_to_Program).first,
        getSymIdByName(EndSym).first
    });

    //std::cout << "hahah";
    // C := {ClosePkg of [S -> ·Program, #]}
    C.push_back(calcuClosePkg(init_closepkg));
    //std::cout << "hoho";
    for (size_t I_idx = 0; I_idx < this->C.size(); I_idx++) {
        const auto I = this->C[I_idx];
        //std::cout << I.id << std::endl;
        for (const auto& X : this->symbol_arr) {
            if (Symbol::isEnd(X) && Symbol::isEpsilon(X))
                continue;
            const ClosePkg dest_close_pkg = GO(I, X);
            // GO 为空
            //std::cout << "sbisbisbis" << std::endl;
            if (dest_close_pkg.LR1_derivation_arr.empty())
                continue;

            //std::cout << "sbsbsbsb" << std::endl;
            // 已经存在，则跳过
            if (isExistClosePkg(dest_close_pkg) != -1) {
                this->goto_table_tmp[{ I.id, X.id }] = isExistClosePkg(dest_close_pkg);
                continue;
            }

            //std::cout << "plusplus" << std::endl;
            // 不存在，那么就往 C 里面添加
            this->C.push_back(dest_close_pkg);
            this->goto_table_tmp[{ I.id, X.id }] = this->C.size()-1;
        }
    }
	//std::cout << std::endl;
}

void LR1Gramma::calcuActionTable() {
    for (const auto& I : this->C) {
        for (const auto& LR0_drv : this->lr0_derivations) {
            for (const auto& terminal : this->terminal_set) {
                if (!this->C[I.id].isExistLR1Drv({ LR0_drv.id, terminal }))
                    continue;

                //std::cout << this->lr0_derivations[LR0_drv.id].left.name << " " << LR0_drv.left.name;
                const derivation_idx drv_idx = LR0_drv.derive_idx;
                const symbol_idx drv_left_idx = LR0_drv.left.id;
                const symbol_idx look_forward_sym_idx = terminal;
                const int drv_dot_position = LR0_drv.dot_position;

                if (drv_dot_position >= LR0_drv.right.size()) {
                    // Origin instead of OriginPro, error in slides
                    if (this->symbol_arr[drv_left_idx].name == Origin)
                        this->action_table[{ I.id, getSymIdByName(EndSym).first }] = { Action::ACC, -1 };
                    else
                        this->action_table[{ I.id, look_forward_sym_idx }] = { Action::R, drv_idx };
                }
                else {
                    const Symbol dot_next_sym = LR0_drv.right[drv_dot_position];
                    // 如果下一个不是非终结符，那就跳过
                    if (!Symbol::isTerminal(dot_next_sym))
                        continue;
                    // 如果找不到，就跳过
                    // 找不到意味着迭代器能够到达 end() 所在位置
                    const auto iter = this->goto_table_tmp.find({ I.id, dot_next_sym.id });
                    if (this->goto_table_tmp.end() == iter)
                        continue;

                    this->action_table[{ I.id, dot_next_sym.id }] = { Action::S, iter->second };
                }
            }
        }
    }
}

void LR1Gramma::calcuGotoTable() {
    for (const auto& I : this->C) {
        for (const auto& LR0_drv : this->lr0_derivations) {
            for (const auto& unterminal : this->unterminal_set) {
                const auto iter = this->goto_table_tmp.find({ I.id, unterminal });
                if (this->goto_table_tmp.end() != iter)
                    this->goto_table[{ I.id, unterminal }] = { Action::R, iter->second };
            }
        }
    }
}