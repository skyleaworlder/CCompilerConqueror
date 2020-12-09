#include "lexer.hpp"
#include "baseGramma.hpp"
#include "LR1.hpp"
#include "utils.hpp"
#include "tree.hpp"

#include <vector>
#include <list>
#include <string>

#include <fstream>
#include <iostream>

int main() {
    const std::vector<std::string> Terminals = {
        "#", "$", "int", "float", "double", "void", "if", "else",
        "while", "return", "+", "-", "*", "/", "=", "<", ">",
        "<=", ">=", "==", "!=", "+=", "-=", "*=", "/=",
        "(", ")", "{", "}", ";", ",", "<ID>", "<INT>"
    };

    std::string to_lex = read_file("code.txt");
    //std::cout << to_lex << std::endl;
    //std::cout << "int main() {    int a = 1;    if ( a == 1 ) {        a += 1;    }    return 0;}" << std::endl;
    Lexer lex(Terminals);
    std::vector<Token> lex_res = lex.parse(to_lex);
    std::list<Token> token_stream(lex_res.begin(), lex_res.end());

    /* debug
    for (const auto& elem : token_stream) {
        std::cout << elem.id << "," << elem.line_number << ","
            << elem.name << ", " << elem.value << std::endl;
    }
    */

    LR1Gramma g;
    g.readGramma("gramma.txt");
    g.calcuAllTerminalFirstSet();
    g.calcuAllUnterminalFirstSet();
    g.calcuLR0Derivations();
    g.calcuLR1Derivations();
    g.calcuActionTable();
    g.calcuGotoTable();

    tree T(
        token_stream, g.derivation_set,
        g.action_table, g.goto_table
    );

    T.print_tree();
}