#include <catch2/catch.hpp>

#include <array>
#include <string>
#include <vector>

#define TESTING
#include <ai/search.h>

template<int NumPlayers>
class TestState {
public:
	static std::string ExpansionOrder;

	int turn;
	int party;
	std::string name;
	float val;

	std::vector<TestState<NumPlayers>> children;
	TestState(int turn, int party, std::string name, float val, std::vector<TestState<NumPlayers>> children) : turn(turn), party(party), name(name), val(val), children(children) {}
	int getGamemode() const { return 0; }
	bool isGameOver() const { return children.size() == 0; }
	std::vector<int> getValidActions() const {
		ExpansionOrder += name;
		std::vector<int> ret;
		for (int i = 0; i < children.size(); i++)
			ret.emplace_back(i);
		return ret;
	}
	TestState<NumPlayers> afterMove(int idx) const {
		return children[idx];
	}
	int getNumPlayers() const { return NumPlayers; }
	int getCurrentParty() const {
		return party;
	}
	int getTurn() const {
		return turn;
	}
};

template<int NumPlayers>
std::string TestState<NumPlayers>::ExpansionOrder;

template<int NumPlayers>
class TestStateMultiValue {
public:
	static std::string ExpansionOrder;

	int turn;
	int party;
	std::string name;
	std::array<float, NumPlayers> val;

	std::vector<TestStateMultiValue<NumPlayers>> children;
	TestStateMultiValue(int turn, int party, std::string name, std::array<float, NumPlayers> val, std::vector<TestStateMultiValue<NumPlayers>> children) : turn(turn), party(party), name(name), val(val), children(children) {}
	int getGamemode() const { return 0; }
	bool isGameOver() const { return children.size() == 0; }
	std::vector<int> getValidActions() const {
		ExpansionOrder += name;
		std::vector<int> ret;
		for (int i = 0; i < children.size(); i++)
			ret.emplace_back(i);
		return ret;
	}
	TestStateMultiValue<NumPlayers> afterMove(int idx) const {
		return children[idx];
	}
	int getNumPlayers() const { return NumPlayers; }
	int getCurrentParty() const {
		return party;
	}
	int getTurn() const {
		return turn;
	}
};
template<int NumPlayers>
std::string TestStateMultiValue<NumPlayers>::ExpansionOrder;

template<int NumPlayers>
class TestEval {
public:
	std::string evalOrder;

	TestEval() {}
	float evaluate(const TestState<NumPlayers>& state, unsigned depth) {
		evalOrder += state.name;
		return state.val;
	}
	std::map<int, float> evaluateAll(const TestStateMultiValue<NumPlayers>& state, unsigned depth) {
		evalOrder += state.name;
		std::map<int, float> ret;
		for (int i = 0; i < state.getNumPlayers(); i++)
			ret[i] = state.val[i];
		return ret;
	}
};

TEST_CASE("Alpha-Beta", "[Search]") {
	using State = TestState<2>;
	using Eval = TestEval<2>;
	using Search = Search<State, int, Eval, int>;
	constexpr auto infty = std::numeric_limits<float>::infinity();
	Eval eval;
	//Graph taken from Wikipedia:
	//https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning#/media/File:AB_pruning.svg
	State K(1, 1, "K", -5, {}); //Real value: 5
	State L(1, 1, "L", -4, {}); //Real value: 4
	State M(1, 1, "M", -3, {}); //Real value: 3
	State N(1, 1, "N", -6, {}); //Real value: 6
	State O(1, 1, "O", -6, {}); //Real value: 6
	State P(1, 1, "P", -7, {}); //Real value: 7
	State Q(1, 1, "Q", -5, {}); //Real value: 5
	State R(1, 1, "R", -8, {}); //Real value: 8
	State S(1, 1, "S", -6, {}); //Real value: 6

	State E(0, 0, "E", 5, {K, L}); //Real value: 5
	State F(0, 0, "F", 3, {M}); //Real value: 3
	State G(0, 0, "G", 6, {N,O}); //Real value: 6
	State H(0, 0, "H", 7, {P}); //Real value: 7
	State I(0, 0, "I", 5, {Q}); //Real value: 5
	State J(0, 0, "J", 8, {R, S}); //Real value: 8
	
	State B(1, 1, "B", -3, {E, F}); //Real value: 3
	State C(1, 1, "C", -4, {G, H}); //Real value: 6
	State D(1, 1, "D", -5, {I, J}); //Real value: 5
	
	State A(0, 0, "A", 4, {B, C, D}); //Real value: 6
	
	State::ExpansionOrder = "";
	eval.evalOrder = "";
	auto score = Search::minimax(A, 0, -infty, infty, eval);
	CHECK(score == 4);
	CHECK(State::ExpansionOrder == "");
	CHECK(eval.evalOrder == "A");
	
	State::ExpansionOrder = "";
	eval.evalOrder = "";
	score = Search::minimax(A, 1, -infty, infty, eval);
	CHECK(score == 5);
	CHECK(State::ExpansionOrder == "A");
	CHECK(eval.evalOrder == "BCD");
	
	State::ExpansionOrder = "";
	eval.evalOrder = "";
	score = Search::minimax(A, 2, -infty, infty, eval);
	CHECK(score == 6);
	CHECK(State::ExpansionOrder == "ABCD");
	CHECK(eval.evalOrder == "EFGHI");
	
	State::ExpansionOrder = "";
	eval.evalOrder = "";
	score = Search::minimax(A, 3, -infty, infty, eval);
	CHECK(score == 6);
	CHECK(State::ExpansionOrder == "ABEFCGHDI");
	CHECK(eval.evalOrder == "KLMNOPQ");
	
	State::ExpansionOrder = "";
	eval.evalOrder = "";
	score = Search::minimax(A, 4, -infty, infty, eval);
	CHECK(score == 6);
	CHECK(State::ExpansionOrder == "ABEFCGHDI");
	CHECK(eval.evalOrder == "KLMNOPQ");
}

TEST_CASE("Alpha-Beta 2v2", "[Search]") {
	using State = TestState<4>;
	using Eval = TestEval<4>;
	using Search = Search<State, int, Eval, int>;
	constexpr auto infty = std::numeric_limits<float>::infinity();
	Eval eval;
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
	State N(0, 0, "N", 10, {}); //Real value: 10
	State O(0, 0, "O",  5, {}); //Real value:  5
	State P(0, 0, "P",  6, {}); //Real value:  6
	State Q(0, 0, "Q",  3, {}); //Real value:  3
	State R(0, 0, "R",  6, {}); //Real value:  6
	State S(0, 0, "S",  2, {}); //Real value:  2
	State T(0, 0, "T",  7, {}); //Real value:  7
	State U(0, 0, "U",  9, {}); //Real value:  9
	State V(0, 0, "V",  8, {}); //Real value:  9

	State H(3, 1, "H", 0, {N,O}); //Real value: 5
	State I(3, 1, "I", 0, {P});   //Real value: 6
	State J(3, 1, "J", 0, {Q,R}); //Real value: 3
	State K(3, 1, "K", 0, {S});   //Real value: 2
	State L(3, 1, "L", 0, {T,U}); //Real value: 7
	State M(3, 1, "M", 0, {V});   //Real value: 8

	State D(2, 1, "D", 0, {H,I}); //Real value: 5
	State E(2, 1, "E", 0, {J,K}); //Real value: 2
	State F(2, 1, "F", 0, {L,M}); //Real value: 7
	State G(2, 1, "G", 1, {});    //Real value: 1
	
	State B(1, 0, "B", 0, {D,E}); //Real value: 5
	State C(1, 0, "C", 0, {F,G}); //Real value: 7

	State A(0, 0, "A", 0, {B,C}); //Real value: 7
	
	State::ExpansionOrder = "";
	eval.evalOrder = "";
	auto score = Search::minimax(A, 4, -infty, infty, eval);
	CHECK(score == 7);
	//CHECK(State::ExpansionOrder == "");
	//CHECK(eval.evalOrder == "A");
}

TEST_CASE("Max^n", "[Search]") {
	using State = TestStateMultiValue<3>;
	using Eval = TestEval<3>;
	using Search = Search<State, int, Eval, int>;
	constexpr auto infty = std::numeric_limits<float>::infinity();
	Eval eval;
	// Tree taken from here:
	// https://www.researchgate.net/figure/An-example-max-n-tree_fig1_224213564
	//               (7,2,9)                    [0]
	//              /        \
	//            /            \
	//      (6,2,6)            (7,2,9)          [1]
	//      /    \             /     \
	// (6,2,6)  (4,1,6)   (7,2,9)   (5,2,1)     [2]
	State D(2, 2, "D", {6,2,6}, {});
	State E(2, 2, "E", {4,1,6}, {});
	State F(2, 2, "F", {7,2,9}, {});
	State G(2, 2, "G", {5,2,1}, {});
	
	State B(1, 1, "B", {0,0,0}, {D,E});
	State C(1, 1, "C", {0,0,0}, {F,G});
	
	State A(0, 0, "A", {0,0,0}, {B,C});
	
	State::ExpansionOrder = "";
	eval.evalOrder = "";
	auto score = Search::maxN(A, 3, eval);
	CHECK(score[0] == 7);
	CHECK(score[1] == 2);
	CHECK(score[2] == 9);
}