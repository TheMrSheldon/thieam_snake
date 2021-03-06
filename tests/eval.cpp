#include <catch2/catch.hpp>

#include <ai/eval.h>

#include <vector>

TEST_CASE("Envbuffer State1", "[Envbuffer]") {
	/**
	 * ╔═══════════════════╗	Snake 1:	<11
	 * ║ . . . . o . . 2 . ║	Snake 2:	<22
	 * ║ . . . . . o . 2 . ║	Food:		o
	 * ║ . 1 1 1 1 1 . 2 . ║	Empty:		.
	 * ║ . . . . . 1 . v . ║
	 * ║ . o . < 1 1 . . . ║
	 * ║ . . . . . . . . o ║
	 * ╚═══════════════════╝
	 */
	std::vector<ls::Position> snake1 = {{3,1},{4,1},{5,1},{5,2},{5,3},{4,3},{3,3},{2,3},{1,3}};
	std::vector<ls::Position> snake2 = {{7,2},{7,3},{7,4},{7,5}};
	std::vector<ls::Position> food = {{8,0},{1,1},{5,4},{4,5}};
	auto s1 = ls::Snake(std::move(snake1), ls::Move::left, 82, ls::SnakeFlags::Player1);
	auto s2 = ls::Snake(std::move(snake2), ls::Move::down, 19, ls::SnakeFlags::Player2);
	auto state = ls::State(9,6, {s1, s2}, std::move(food), {});
	EnvBuffer env(state.getNumSnakes(), state.getWidth(), state.getHeight());

	SECTION("Clearing") {
		env.clear();
		for (unsigned x = 0; x < state.getWidth(); x++) {
			for (unsigned y = 0; y < state.getHeight(); y++) {
				CHECK_FALSE(env.isBlockedAtTurn(state, ls::Position(x,y), 0, 0));
				CHECK_FALSE(env.isBlockedAtTurn(state, ls::Position(x,y), 1, 0));
			}
		}
	//}
	//SECTION("Fields blocked by snakes") {
		env.storeSnake(0, s1);
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[0], 0, 0));
		//Last bodypart
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 0));
		CHECK_FALSE(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 1));
		CHECK_FALSE(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 2));
		//Second last bodypart
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[7], 0, 0));
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[7], 0, 1));
		CHECK_FALSE(env.isBlockedAtTurn(state, s1.getBody()[7], 0, 2));
		CHECK_FALSE(env.isBlockedAtTurn(state, s1.getBody()[7], 0, 3));
	//}
	//SECTION("Storing blocked fields") {
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 0));
		CHECK_FALSE(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 3));
		CHECK_FALSE(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 4));
		CHECK_FALSE(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 5));
		//Overwrite the field initially blocked by the snakes tail
		env.blockAfterTurn(s1.getBody()[8], 0, 4);
		//This check should be illegal since snake 0 already claimed this field
		//CHECK_FALSE(env.isBlockedAtTurn(s1.getBody()[8], 0, 3));
		//This check should be illegal since snake 0 already claimed this field
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 4));
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 5));
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[8], 0, 6));
		//This is a border-tile and thus part of the territory of snake 0 since it is longer
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[8], 1, 4));
		CHECK(env.isBlockedAtTurn(state, s1.getBody()[8], 1, 5));
	}
}
TEST_CASE("Evaluate State1", "[Evaluation]") {
	/**
	 * ╔═══════════════════╗	Snake 1:	<11
	 * ║ . . . . o . . 2 . ║	Snake 2:	<22
	 * ║ . . . . . o . 2 . ║	Food:		o
	 * ║ . 1 1 1 1 1 . 2 . ║	Empty:		.
	 * ║ . . . . . 1 . v . ║
	 * ║ . o . < 1 1 . . . ║
	 * ║ . . . . . . . . o ║
	 * ╚═══════════════════╝
	 */
	std::vector<ls::Position> snake1 = {{3,1},{4,1},{5,1},{5,2},{5,3},{4,3},{3,3},{2,3},{1,3}};
	std::vector<ls::Position> snake2 = {{7,2},{7,3},{7,4},{7,5}};
	std::vector<ls::Position> food = {{8,0},{1,1},{5,4},{4,5}};
	auto s1 = ls::Snake(std::move(snake1), ls::Move::left, 82, ls::SnakeFlags::Player1);
	auto s2 = ls::Snake(std::move(snake2), ls::Move::down, 19, ls::SnakeFlags::Player2);
	auto state = ls::State(9,6, {s1, s2}, std::move(food), {});
	
	//Assert correct evaluation
	StateOfMind mind(0);
	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	for (auto& snake : state.getSnakes())
		mindmap.insert({snake.getSquad(), mind});
	Evaluator evaluator(ls::gm::Standard, state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	auto eval = evaluator.evaluate(state, 1);
	CHECK(eval.winner == ls::SnakeFlags::None);
	REQUIRE(eval.snakes.size() == 2);
	
	/* Board:
	 * ╔═══════════════════╗	Snake 1:	<11
	 * ║ . . . . o . . 2 . ║	Snake 2:	<22
	 * ║ . . . . . o . 2 . ║	Food:		o
	 * ║ . 1 1 1 1 1 . 2 . ║	Empty:		.
	 * ║ . . . . . 1 . v . ║
	 * ║ . o . < 1 1 . . . ║
	 * ║ . . . . . . . . o ║
	 * ╚═══════════════════╝
	 * 
	 * Iterations:
	 * 0:                   	1:                   	2:
	 * ╔═══════════════════╗	╔═══════════════════╗	╔═══════════════════╗
	 * ║ . . . . o . . 1 . ║	║ . . . . o . . 1 . ║	║ . . . . o . . 1 . ║
	 * ║ . . . . . o . 2 . ║	║ . . . . . o . 2 . ║	║ . . . . . o . 2 . ║
	 * ║ . 1 2 3 4 5 . 3 . ║	║ . 1 2 3 4 5 . 3 . ║	║ . 1 2 3 4 5 ▒ 3 ▒ ║
	 * ║ . . . . . 6 . 4 . ║	║ . . . ▓ . 6 ▒ 4 ▒ ║	║ . . ▓ █ ▓ 6 ░ 4 ░ ║
	 * ║ . o . 9 8 7 . . . ║	║ . o ▓ 9 8 7 . ▒ . ║	║ . ▓ █ 9 8 7 ▒ ░ ▒ ║
	 * ║ . . . . . . . . o ║	║ . . . ▓ . . . . o ║	║ . . ▓ █ ▓ . . ▒ o ║
	 * ╚═══════════════════╝	╚═══════════════════╝	╚═══════════════════╝
	 * 3:                   	4:                   	5:
	 * ╔═══════════════════╗	╔═══════════════════╗	╔═══════════════════╗
	 * ║ . . . . o . . 1 . ║	║ . . . . o . ▒ 1 ▒ ║	║ . . ▓ . o ▒ ░ ▒ ░ ║
	 * ║ . . . . . o ▒ 2 ▒ ║	║ . . ▓ . . ▒ ░ ▒ ░ ║	║ . ▓ █ ▓ ▒ ░ ░ ░ ░ ║
	 * ║ . 1 ▓ 3 4 5 ░ ▒ ░ ║	║ . ▓ █ ▓ 4 5 ░ ░ ░ ║	║ ▓ █ █ █ ▓ ▒ ░ ░ ░ ║
	 * ║ . ▓ █ █ █ 6 ░ 4 ░ ║	║ ▓ █ █ █ █ 6 ░ ▒ ░ ║	║ █ █ █ █ █ 6 ░ ░ ░ ║
	 * ║ ▓ █ █ 9 8 7 ░ ░ ░ ║	║ █ █ █ 9 8 7 ░ ░ ░ ║	║ █ █ █ 9 8 7 ░ ░ ░ ║
	 * ║ . ▓ █ █ █ ▓ ▒ ░ ▒ ║	║ ▓ █ █ █ █ █ ░ ░ ░ ║	║ █ █ █ █ █ █ ░ ░ ░ ║
	 * ╚═══════════════════╝	╚═══════════════════╝	╚═══════════════════╝
	 * 6:                   	7:                   	8:
	 * ╔═══════════════════╗	╔═══════════════════╗	╔═══════════════════╗
	 * ║ . ▓ █ ▓ ▒ ░ ░ ░ ░ ║	║ ▓ █ █ █ ░ ░ ░ ░ ░ ║	║ █ █ █ █ ░ ░ ░ ░ ░ ║
	 * ║ ▓ █ █ █ ░ ░ ░ ░ ░ ║	║ █ █ █ █ ░ ░ ░ ░ ░ ║	║ █ █ █ █ ░ ░ ░ ░ ░ ║
	 * ║ █ █ █ █ █ ░ ░ ░ ░ ║	║ █ █ █ █ █ ░ ░ ░ ░ ║	║ █ █ █ █ █ ░ ░ ░ ░ ║
	 * ║ █ █ █ █ █ ▒ ░ ░ ░ ║	║ █ █ █ █ █ ░ ░ ░ ░ ║	║ █ █ █ █ █ ░ ░ ░ ░ ║
	 * ║ █ █ █ 9 8 7 ░ ░ ░ ║	║ █ █ █ 9 8 ▒ ░ ░ ░ ║	║ █ █ █ 9 ▒ ░ ░ ░ ░ ║
	 * ║ █ █ █ █ █ █ ░ ░ ░ ║	║ █ █ █ █ █ █ ░ ░ ░ ║	║ █ █ █ █ █ █ ░ ░ ░ ║
	 * ╚═══════════════════╝	╚═══════════════════╝	╚═══════════════════╝
	 * 9:                   	10:
	 * ╔═══════════════════╗	╔═══════════════════╗	▓:	Frontier Snake0
	 * ║ █ █ █ █ ░ ░ ░ ░ ░ ║	║ █ █ █ █ ░ ░ ░ ░ ░ ║	▒:	Frontier Snake1
	 * ║ █ █ █ █ ░ ░ ░ ░ ░ ║	║ █ █ █ █ ░ ░ ░ ░ ░ ║	█:	Control Snake0
	 * ║ █ █ █ █ █ ░ ░ ░ ░ ║	║ █ █ █ █ █ ░ ░ ░ ░ ║	░:	Control Snake1
	 * ║ █ █ █ █ █ ░ ░ ░ ░ ║	║ █ █ █ █ █ ░ ░ ░ ░ ║	X:	Fontier Border
	 * ║ █ █ █ ▒ ░ ░ ░ ░ ░ ║	║ █ █ █ ░ ░ ░ ░ ░ ░ ║	x:	Control Border
	 * ║ █ █ █ █ █ █ ░ ░ ░ ║	║ █ █ █ █ █ █ ░ ░ ░ ║
	 * ╚═══════════════════╝	╚═══════════════════╝
	 * 
	 * Result:
	 * 	Snake | Area control | Border control
	 * 	=====================================
	 * 	0    |           27 |             0
	 * 	1    |           27 |             0
	 */
	CHECK(eval.snakes[0].mobility == 27);
	CHECK(eval.snakes[1].mobility == 27);
	CHECK(eval.snakes[0].border == 0);
	CHECK(eval.snakes[1].border == 0);
	CHECK(eval.snakes[0].foodInReach == 1);
	CHECK(eval.snakes[1].foodInReach == 3);
}


TEST_CASE("Evaluate State2", "[Evaluation]") {
	/**
	 * ╔═════════╗	Snake 1:	<11
	 * ║ . . . . ║	Snake 2:	<22
	 * ║ . 2 > . ║	Food:		o
	 * ║ . . 1 > ║	Empty:		.
	 * ║ . . 1 . ║
	 * ╚═════════╝
	 */
	std::vector<ls::Position> snake1 = {{3,1}, {2,1}, {2,0}};
	std::vector<ls::Position> snake2 = {{2,2}, {1,2}};
	std::vector<ls::Position> food = {};
	auto s1 = ls::Snake(std::move(snake1), ls::Move::up, 100, ls::SnakeFlags::Player1);
	auto s2 = ls::Snake(std::move(snake2), ls::Move::right, 100, ls::SnakeFlags::Player2);
	auto state = ls::State(4,4, {s1, s2}, std::move(food), {});

	//Assert correct setup
	REQUIRE(state.getWidth() == 4);
	REQUIRE(state.getHeight() == 4);
	REQUIRE(state.getNumSnakes() == 2);
	REQUIRE(state.getSnake(0).getHeadPos() == ls::Position(3,1));
	REQUIRE(state.getSnake(1).getHeadPos() == ls::Position(2,2));

	//Assert correct evaluation
	StateOfMind mind(0);
	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	for (auto& snake : state.getSnakes())
		mindmap.insert({snake.getSquad(), mind});
	Evaluator evaluator(ls::gm::Standard, state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	auto eval = evaluator.evaluate(state, 1);
	CHECK(eval.winner == ls::SnakeFlags::None);
	REQUIRE(eval.snakes.size() == 2);
	/** Board
	 * ╔═════════╗	Snake 1:	<11
	 * ║ . . . . ║	Snake 2:	<22
	 * ║ . 2 > . ║	Food:		o
	 * ║ . . 1 > ║	Empty:		.
	 * ║ . . 1 . ║
	 * ╚═════════╝
	 * 
	 * Iterations:
	 * 0:         	1:         	2:         	3:         	4:         	▓:	Frontier Snake0
	 * ╔═════════╗	╔═════════╗	╔═════════╗	╔═════════╗	╔═════════╗	▒:	Frontier Snake1
	 * ║ . . . . ║	║ . . ▒ . ║	║ . ▒ ░ x ║	║ ▒ ░ ░ X ║	║ ░ ░ ░ X ║	█:	Control Snake0
	 * ║ . 1 2 . ║	║ . ▒ ░ x ║	║ ▒ ░ ░ X ║	║ ░ ░ ░ X ║	║ ░ ░ ░ X ║	░:	Control Snake1
	 * ║ . . 2 3 ║	║ . . 2 █ ║	║ . ▒ 2 █ ║	║ ▒ ░ x █ ║	║ ░ ░ X █ ║	X:	Fontier Border
	 * ║ . . 1 . ║	║ . . 1 ▓ ║	║ . . ▓ █ ║	║ . x █ █ ║	║ x X █ █ ║	x:	Control Border
	 * ╚═════════╝	╚═════════╝	╚═════════╝	╚═════════╝	╚═════════╝
	 * 
	 * Result:
	 * 	Snake | Area control | Border control
	 * 	=====================================
	 * 	0    |             3 |             5
	 * 	1    |             8 |             5
	 *  Since snake 1 is longer all border control should be added to its area control
	 */
	CHECK(eval.snakes[0].mobility == 8);
	CHECK(eval.snakes[1].mobility == 8);
	CHECK(eval.snakes[0].border == 0);
	CHECK(eval.snakes[1].border == 0);
	CHECK(eval.snakes[0].foodInReach == 0);
	CHECK(eval.snakes[1].foodInReach == 0);
}

TEST_CASE("Evaluate State3", "[Evaluation]") {
	/**
	 * ╔═════════╗	Snake 1:	<11
	 * ║ . . . . ║	Snake 2:	<22
	 * ║ . 2 > . ║	Food:		o
	 * ║ . . 1 > ║	Empty:		.
	 * ║ 1 1 1 . ║
	 * ╚═════════╝
	 */
	std::vector<ls::Position> snake1 = {{3,1}, {2,1}, {2,0}, {1,0}, {0,0}};
	std::vector<ls::Position> snake2 = {{2,2}, {1,2}};
	std::vector<ls::Position> food = {};
	auto s1 = ls::Snake(std::move(snake1), ls::Move::up, 100, ls::SnakeFlags::Player1);
	auto s2 = ls::Snake(std::move(snake2), ls::Move::right, 100, ls::SnakeFlags::Player2);
	auto state = ls::State(4,4, {s1, s2}, std::move(food), {});

	//Assert correct setup
	REQUIRE(state.getWidth() == 4);
	REQUIRE(state.getHeight() == 4);
	REQUIRE(state.getNumSnakes() == 2);
	REQUIRE(state.getSnake(0).getHeadPos() == ls::Position(3,1));
	REQUIRE(state.getSnake(1).getHeadPos() == ls::Position(2,2));

	//Assert correct evaluation
	StateOfMind mind(0);
	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	for (auto& snake : state.getSnakes())
		mindmap.insert({snake.getSquad(), mind});
	Evaluator evaluator(ls::gm::Standard, state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	auto eval = evaluator.evaluate(state, 1);
	CHECK(eval.winner == ls::SnakeFlags::None);
	REQUIRE(eval.snakes.size() == 2);
	/** Board
	 * ╔═════════╗	Snake 1:	<11
	 * ║ . . . . ║	Snake 2:	<22
	 * ║ . 2 > . ║	Food:		o
	 * ║ . . 1 > ║	Empty:		.
	 * ║ 1 1 1 . ║
	 * ╚═════════╝
	 * 
	 * Iterations:
	 * 0:         	1:         	2:         	3:         	4:         
	 * ╔═════════╗	╔═════════╗	╔═════════╗	╔═════════╗	╔═════════╗
	 * ║ . . . . ║	║ . . ▒ . ║	║ . ▒ ░ x ║	║ ▒ ░ ░ X ║	║ ░ ░ ░ X ║
	 * ║ . 1 2 . ║	║ . ▒ ░ x ║	║ ▒ ░ ░ X ║	║ ░ ░ ░ X ║	║ ░ ░ ░ X ║
	 * ║ . . 4 5 ║	║ . . 4 █ ║	║ . ▒ 4 █ ║	║ ▒ ░ 4 █ ║	║ ░ ░ 4 █ ║
	 * ║ 1 2 3 . ║	║ 1 2 3 ▓ ║	║ 1 2 3 █ ║	║ 1 ▒ 3 █ ║	║ ▒ ░ ▒ █ ║
	 * ╚═════════╝	╚═════════╝	╚═════════╝	╚═════════╝	╚═════════╝
	 * 5:         	6:         	▓:	Frontier Snake0
	 * ╔═════════╗	╔═════════╗	▒:	Frontier Snake1
	 * ║ ░ ░ ░ X ║	║ ░ ░ ░ X ║	█:	Control Snake0
	 * ║ ░ ░ ░ X ║	║ ░ ░ ░ X ║	░:	Control Snake1
	 * ║ ░ ░ ▒ █ ║	║ ░ ░ ░ █ ║	X:	Fontier Border
	 * ║ ░ ░ ░ █ ║	║ ░ ░ ░ █ ║	x:	Control Border
	 * ╚═════════╝	╚═════════╝
	 * 
	 * Result:
	 * 	Snake | Area control | Border control
	 * 	=====================================
	 * 	0    |             2 |             2
	 * 	1    |            12 |             2
	 *  Since snake 1 is longer all border control should be added to its area control
	 */
	CHECK(eval.snakes[0].mobility == 4);
	CHECK(eval.snakes[1].mobility == 12);
	CHECK(eval.snakes[0].border == 0);
	CHECK(eval.snakes[1].border == 0);
	CHECK(eval.snakes[0].foodInReach == 0);
	CHECK(eval.snakes[1].foodInReach == 0);
}