#include "utils.hpp"
#include "syntaxParser.hpp"
#include <string>
#include <vector>
#include <set>
#include <iostream>

void test_split() {
    std::string a = "A -> BC | cd | bc";
    std::string b = " | ";
    std::string c = " -> ";

    std::vector<std::string> left_right = split(a, c);
    for (auto elem : left_right)
        std::cout << "(" << elem << ")" << std::endl;

    std::vector<std::string> right_arr = split(left_right[1], b);
    for (auto elem : right_arr)
        std::cout << "(" << elem << ")" << std::endl;
}

void test_trim() {
    using std::string;
    using strarr = std::vector<std::string>;
    string a = " sa  | sada   -> as ";
    strarr arr = split(a, "->");

    for (auto elem : arr) {
        std::cout << "(" << elem << ")" << std::endl;
        std::cout << "(" << trim(elem) << ")" << std::endl;
    }

}

void test_set() {
    using std::set;
    struct haha {
        int a; int b;
        haha(int a, int b) : a(a), b(b) {};
        bool operator<(const haha& rhs) const {
            return a < rhs.a;
        }
    };
    set<haha> a;
    a.insert(haha(1,2));
    a.insert(haha(1,2));
    a.insert(haha(3,4));
    a.insert(haha(4,3));
    using std::cout;
    using std::endl;
    for (auto elem : a)
        printf("%d, %d\n", elem.a, elem.b);
}

void test_string() {
    using std::cout;
    using std::endl;
    using std::string;
    string a = "456";
    string b = "456";
    cout << (a == b) << endl;
}

void test_readgramma() {
    using std::cout;
    using std::endl;
    BaseGrammar g;
    g.readGramma("./gramma.txt");

    cout << "symbol set: ";
    for (const auto& elem : g.symbol_arr)
        cout << elem.name << " ";
    cout << endl;

    cout << "terminal set: ";
    for (const auto& elem : g.terminal_set)
        cout << elem << " ";
    cout << endl;

    cout << "unterminal set: ";
    for (const auto& elem : g.unterminal_set)
        cout << "(" << elem << "); ";
    cout << endl;

    cout << "derivation vector: " << endl;
	cout << g.derivation_set.size() << endl;
    for (auto elem : g.derivation_set) {
		cout << "left symbol: " << elem.left.id << " " << elem.left.name << endl;
        cout << "right symbol: ";
        for (auto flfm : elem.right)
            cout << flfm.name << " ";
        cout << endl << endl;
    }
}

void test_first() {
	using std::cout;
	using std::endl;
	BaseGrammar g;
	g.readGramma("./gramma.txt");

	g.calcuAllTerminalFirstSet();
	g.calcuAllUnterminalFirstSet();
	cout << "first set: ";
	for (const auto &elem : g.symbol_arr) {
		cout << elem.id << ": " << elem.name << endl;
		cout << "first set size: " << elem.FIRST_SET.size() << endl;
		for (const auto & flfm : elem.FIRST_SET)
			cout << "(" << flfm << ", " << g.symbol_arr[flfm].name << ")" << endl;
	}
}

int main() {
    //test_split();
    //test_set();
    //test_trim();
    //test_readgramma();
    //test_string();

	test_first();
}