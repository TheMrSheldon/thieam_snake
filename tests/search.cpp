#include <catch2/catch.hpp>

#include <string>
#include <vector>

#define TESTING
#include <ai/search.h>

class TestState {
public:
	static std::string ExpansionOrder;

	int turn;
    int party;
	std::string name;
	float val;
	std::vector<TestState> children;
	TestState(int turn, int party, std::string name, float val, std::vector<TestState> children) : turn(turn), party(party), name(name), val(val), children(children) {}
	int getGamemode() const { return 0; }
	bool isGameOver() const { return children.size() == 0; }
	std::vector<int> getValidActions() const {
		ExpansionOrder += name;
		std::vector<int> ret;
		for (int i = 0; i < children.size(); i++)
			ret.emplace_back(i);
		return ret;
	}
	TestState afterMove(int idx) const {
		return children[idx];
	}
	int getNumPlayers() const { return 2; }
    int getCurrentParty() const {
        return party;
    }
	int getTurn() const {
		return turn;
	}
};
std::string TestState::ExpansionOrder = "";

class TestEval {
public:
	std::string evalOrder;

	TestEval() {}
	float evaluate(const TestState& state, unsigned depth) {
		evalOrder += state.name;
		return state.val;
	}
};

TEST_CASE("Alpha-Beta", "[Search]") {
	using Search = Search<TestState, int, TestEval, int>;
    constexpr auto infty = std::numeric_limits<float>::infinity();
	TestEval eval;
	//Graph taken from Wikipedia:
	//https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning#/media/File:AB_pruning.svg
	TestState K(1, 1, "K", -5, {}); //Real value: 5
	TestState L(1, 1, "L", -4, {}); //Real value: 4
	TestState M(1, 1, "M", -3, {}); //Real value: 3
	TestState N(1, 1, "N", -6, {}); //Real value: 6
	TestState O(1, 1, "O", -6, {}); //Real value: 6
	TestState P(1, 1, "P", -7, {}); //Real value: 7
	TestState Q(1, 1, "Q", -5, {}); //Real value: 5
	TestState R(1, 1, "R", -8, {}); //Real value: 8
	TestState S(1, 1, "S", -6, {}); //Real value: 6

	TestState E(0, 0, "E", 5, {K, L}); //Real value: 5
	TestState F(0, 0, "F", 3, {M}); //Real value: 3
	TestState G(0, 0, "G", 6, {N,O}); //Real value: 6
	TestState H(0, 0, "H", 7, {P}); //Real value: 7
	TestState I(0, 0, "I", 5, {Q}); //Real value: 5
	TestState J(0, 0, "J", 8, {R, S}); //Real value: 8
	
	TestState B(1, 1, "B", -3, {E, F}); //Real value: 3
	TestState C(1, 1, "C", -4, {G, H}); //Real value: 6
	TestState D(1, 1, "D", -5, {I, J}); //Real value: 5

	TestState A(0, 0, "A", 4, {B, C, D}); //Real value: 6
    
	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
    auto score = Search::minimax(A, 0, -infty, infty, eval);
    CHECK(score == 4);
    CHECK(TestState::ExpansionOrder == "");
    CHECK(eval.evalOrder == "A");

	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
    score = Search::minimax(A, 1, -infty, infty, eval);
    CHECK(score == 5);
    CHECK(TestState::ExpansionOrder == "A");
    CHECK(eval.evalOrder == "BCD");

	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
    score = Search::minimax(A, 2, -infty, infty, eval);
    CHECK(score == 5);
    CHECK(TestState::ExpansionOrder == "ABCD");
    CHECK(eval.evalOrder == "EFGHI");

	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
    score = Search::minimax(A, 3, -infty, infty, eval);
	CHECK(score == 5);
	CHECK(TestState::ExpansionOrder == "ABEFCGHDI");
	CHECK(eval.evalOrder == "KLMNOPQ");
    
	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
    score = Search::minimax(A, 4, -infty, infty, eval);
	CHECK(score == 5);
	CHECK(TestState::ExpansionOrder == "ABEFCGHDI");
	CHECK(eval.evalOrder == "KLMNOPQ");
}

TEST_CASE("Alpha-Beta 2v2", "[Search]") {
	using Search = Search<TestState, int, TestEval, int>;
    constexpr auto infty = std::numeric_limits<float>::infinity();
	TestEval eval;
    // 2v2 Alpha-Beta search on the following Graph:
    //               7                          MAX
    //            /     \
    //          /         \
    //        5             7                   MAX
    //       /  \           |\
    //     /      \         | \
    //    5        2        7  1                MIN
    //   / \      / \      / \
    //   5  6     3  2     7  8                 MIN
    //  / \  \   / \  \   / \  \
    // 10  5  6  3  6  2  7  9  8
	TestState N(0, 0, "N", 10, {}); //Real value: 10
	TestState O(0, 0, "O",  5, {}); //Real value:  5
	TestState P(0, 0, "P",  6, {}); //Real value:  6
	TestState Q(0, 0, "Q",  3, {}); //Real value:  3
	TestState R(0, 0, "R",  6, {}); //Real value:  6
	TestState S(0, 0, "S",  2, {}); //Real value:  2
	TestState T(0, 0, "T",  7, {}); //Real value:  7
	TestState U(0, 0, "U",  9, {}); //Real value:  9
	TestState V(0, 0, "V",  8, {}); //Real value:  9

	TestState H(3, 1, "H", 0, {N,O}); //Real value: 5
	TestState I(3, 1, "I", 0, {P});   //Real value: 6
	TestState J(3, 1, "J", 0, {Q,R}); //Real value: 3
    TestState K(3, 1, "K", 0, {S});   //Real value: 2
	TestState L(3, 1, "L", 0, {T,U}); //Real value: 7
	TestState M(3, 1, "M", 0, {V});   //Real value: 8

	TestState D(2, 1, "D", 0, {H,I}); //Real value: 5
	TestState E(2, 1, "E", 0, {J,K}); //Real value: 2
	TestState F(2, 1, "F", 0, {L,M}); //Real value: 7
	TestState G(2, 1, "G", 1, {});    //Real value: 1
	
	TestState B(1, 0, "B", 0, {D,E}); //Real value: 5
	TestState C(1, 0, "C", 0, {F,G}); //Real value: 7

	TestState A(0, 0, "A", 0, {B,C}); //Real value: 7
    
	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
    auto score = Search::minimax(A, 4, -infty, infty, eval);
    CHECK(score == 7);
    //CHECK(TestState::ExpansionOrder == "");
    //CHECK(eval.evalOrder == "A");
}