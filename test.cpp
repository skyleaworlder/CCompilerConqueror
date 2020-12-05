#include "utils.hpp"
#include "baseGramma.hpp"
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
    using std::cout;
    using std::endl;
    struct haha {
        int a; int b;
        haha(int a, int b) : a(a), b(b) {};
        bool operator<(const haha& rhs) const {
            return a < rhs.a;
        }
        bool operator==(const haha& rhs) const {
            return (a==rhs.a) && (b==rhs.b);
        }
    };
    set<haha> a;
    a.insert(haha(1,2));
    a.insert(haha(1,2));
    a.insert(haha(3,4));
    a.insert(haha(4,3));
    for (auto elem : a)
        printf("%d, %d\n", elem.a, elem.b);

    set<int> un;
    un.insert(2);
    for (auto elem : un)
        cout << elem << " ";

    set<haha> b;
    b.insert(haha(1,2));
    b.insert(haha(4,3));
    b.insert(haha(3,4));
    for (auto elem : b)
        printf("%d, %d\n", elem.a, elem.b);
    cout << "is equal:" << (a==b) << endl;
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
    g.readGramma("./testgramma.txt");

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
        cout << "derive number: " << elem.id << endl;
        cout << "left symbol: " << elem.left.id << " " << elem.left.name << endl;
        cout << "right symbol: ";
        for (auto flfm : elem.right)
            cout << flfm.name << " ";
        cout << endl << endl;
    }
}

void test_equal() {
    using std::string;
    using std::vector;
    using std::cout;
    using std::endl;
    string a = "haha";
    string b = "hoho";
    Symbol haha(a, Symbol::SYM_TYPE::UNTERMINAL, 0);
    Symbol hoho(b, Symbol::SYM_TYPE::TERMINAL, 1);
    Symbol hihi(b, Symbol::SYM_TYPE::TERMINAL, 1);
    Derivation a_d(haha, vector<Symbol>{hoho}, false);
    Derivation b_d(haha, vector<Symbol>{hihi}, false);

    std::cout << "derive is equal: " << (a_d == b_d) << std::endl;
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

void test_cpp_static_method() {
    Symbol a("haha", Symbol::SYM_TYPE::TERMINAL, 0);
    Symbol b("$", Symbol::SYM_TYPE::EPSILON, 1);
    Symbol c("#", Symbol::SYM_TYPE::END, 2);
    Symbol d("HAHA", Symbol::SYM_TYPE::UNTERMINAL, 3);

    std::vector<Symbol> arr { a,b,c,d };
    for (const auto& elem : arr) {
        std::cout << elem.name << " is terminal?: "<< Symbol::isTerminal(elem) << std::endl;
        std::cout << elem.name << " is unterminal?: "<< Symbol::isUnTerminal(elem) << std::endl;
        std::cout << elem.name << " is epsilon?: "<< Symbol::isEpsilon(elem) << std::endl;
        std::cout << elem.name << " is end?: "<< Symbol::isEnd(elem) << std::endl;
    }
}

int main() {
    //test_split();
    //test_set();
    //test_trim();
    //test_readgramma();
    //test_string();
    //test_equal();
	//test_first();
    test_cpp_static_method();
}