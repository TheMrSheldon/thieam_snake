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
		printf("%s\n", name.c_str());
		fflush(stdout);
		ExpansionOrder += name+" ";
		std::vector<int> ret;
		for (int i = 0; i < children.size(); i++)
			ret.emplace_back(i);
		return ret;
	}
	TestState afterMove(int idx) const {
		printf("[%i] -> %s\n", idx, children[idx].name.c_str());
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
	TestState E(0, "E", 5, {}); //Real value: 5
	TestState F(0, "F", 3, {}); //Real value: 3
	TestState G(0, "G", 6, {}); //Real value: 6
	TestState H(0, "H", 7, {}); //Real value: 7
	TestState I(0, "I", 5, {}); //Real value: 5
	TestState J(0, "J", 8, {}); //Real value: 8
	
	TestState B(1, "B", 3, {E, F}); //Real value: 3
	TestState C(1, "C", 6, {G, H}); //Real value: 6
	TestState D(1, "D", 5, {I, J}); //Real value: 5

	TestState A(0, "A", 4, {B, C, D}); //Real value: 6

	auto routine = Search::iterativeDeepening(A, 2, eval, false);
	REQUIRE((bool)routine);
	CHECK(routine() == 6);
	printf("%s\n", TestState::ExpansionOrder.c_str());
	printf("%s\n", eval.evalOrder.c_str());
	
	TestState::ExpansionOrder = "";
	eval.evalOrder = "";
	REQUIRE((bool)routine);
	//CHECK(routine() == 6);
	printf("%s\n", TestState::ExpansionOrder.c_str());
	printf("%s\n", eval.evalOrder.c_str());

	CHECK(false);
}