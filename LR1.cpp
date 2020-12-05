#include "LR1.hpp"

void LR1Gramma::calcuLR0Derivations() {
    for (size_t index = 0; index < this->derivation_set.size(); ++index) {
        for (size_t dot_pos = 0; dot_pos < this->derivation_set[index].right.size(); ++dot_pos) {
            this->lr0_derivations.emplace_back(this->derivation_set[index]);
            // back point to the last position
            // while end point point to the next pos of back()
            this->lr0_derivations.back().dot_position = dot_pos;
            this->lr0_derivations.back().lr1_flag = true;
            // derive_idx equal -1 when construct Derivation
            // while id is AUTO_INCREMENT
            this->lr0_derivations.back().derive_idx = this->derivation_set[index].id;
        }
    }
}