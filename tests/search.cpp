#include <catch2/catch.hpp>

#include <string>
#include <vector>

#define TESTING
#include <ai/search.h>

class TestState {
public:
	static std::string ExpansionOrder;

	int turn;
	std::string name;
	float val;
	std::vector<TestState> children;
	TestState(int turn, std::string name, float val, std::vector<TestState> children) : turn(turn), name(name), val(val), children(children) {}
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
	int getWidth() const { return 0; }
	int getHeight() const { return 0; }
	int getTurn() const {
		return turn;
	}
};
std::string TestState::ExpansionOrder = "";

class TestEval {
public:
	std::string evalOrder;

	TestEval(int gamemode, int numplayers, int width, int height) {}
	float evaluate(const TestState& state) {
		evalOrder += state.name;
		return state.val;
	}
};

TEST_CASE("Iterative Deepening", "[Search]") {
	using Search = Search<TestState, int, TestEval>;
	TestEval eval(0, 2, 0, 0);
	//Graph taken from Wikipedia:
	//https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning#/media/File:AB_pruning.svg
	TestState K(1, "K", 5, {}); //Real value: 5
	TestState L(1, "L", 4, {}); //Real value: 4
	TestState M(1, "M", 3, {}); //Real value: 3
	TestState N(1, "N", 6, {}); //Real value: 6
	TestState O(1, "O", 6, {}); //Real value: 6
	TestState P(1, "P", 7, {}); //Real value: 7
	TestState Q(1, "Q", 5, {}); //Real value: 5
	TestState R(1, "R", 8, {}); //Real value: 8
	TestState S(1, "S", 6, {}); //Real value: 6

	TestState E(0, "E", 5, {K, L}); //Real value: 5
	TestState F(0, "F", 3, {M}); //Real value: 3
	TestState G(0, "G", 6, {N,O}); //Real value: 6
	TestState H(0, "H", 7, {P}); //Real value: 7
	TestState I(0, "I", 5, {Q}); //Real value: 5
	TestState J(0, "J", 8, {R, S}); //Real value: 8
	
	TestState B(1, "B", 3, {E, F}); //Real value: 3
	TestState C(1, "C", 6, {G, H}); //Real value: 6
	TestState D(1, "D", 5, {I, J}); //Real value: 5

	TestState A(0, "A", 4, {B, C, D}); //Real value: 6

	auto routine = Search::iterativeDeepening(A, 2, eval);
	REQUIRE((bool)routine);
	CHECK(routine() == 6);
	CHECK(TestState::ExpansionOrder == "ABCD");
	CHECK(eval.evalOrder == "EFGHI");
	
	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
	REQUIRE((bool)routine);
	CHECK(routine() == 5);
	CHECK(TestState::ExpansionOrder == "ABEFCGHDI");
	CHECK(eval.evalOrder == "KLMNOPQ");
}