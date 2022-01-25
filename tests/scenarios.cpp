/**
 * @file scenarios.cpp
 * @brief Testcases with scenarios for which the correct move is obvious.
 * @details This file contains scenarios and their best move for at least
 * one party correct move is obvious and should easily be where the found
 * by the tree-search-algorithm or heuristic. These scenarios mostly
 * consist of real examples where the AI took a wrong move but where it
 * should have been obvious to the AI that the move is losing (e.g. since
 * it leads to a dead end).
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#define TESTING
#include <ai/eval.h>
#include <ai/search.h>
#include <ai/state_of_mind.h>

#include <libsnake/snake.h>
#include <libsnake/state.h>
#include <libsnake/gamemodes/squad.h>

#include <map>

TEST_CASE("Scenario 1", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ o . . . o . o . . o o ║	Snake 2:	<22
	 * ║ . . . . . o . 2 2 o . ║	Food:		o
	 * ║ o . . . o . . . 2 . . ║	Empty:		.
	 * ║ o . o . . . . . 2 2 . ║
	 * ║ o . . . 1 1 1 . 2 2 . ║
	 * ║ o . . . 1 1 1 . 2 . . ║
	 * ║ o . . . 1 . . . 2 . . ║
	 * ║ . . . . 1 . . . 2 . . ║
	 * ║ o . . . 1 1 . . 2 2 > ║
	 * ║ . o . o 1 1 . . . ^ . ║
	 * ║ o . . . 1 1 1 1 1 1 o ║
	 * ╚═══════════════════════╝
	 */
	std::vector<ls::Position> snake1 = {{9,1},{9,0},{8,0},{7,0},{6,0},{5,0},{4,0},{4,1},{5,1},{5,2},{4,2},{4,3},{4,4},{4,5},{4,6},{5,6},{6,6},{6,5},{5,5}};
	std::vector<ls::Position> snake2 = {{10,2},{9,2},{8,2},{8,3},{8,4},{8,5},{8,6},{9,6},{9,7},{8,7},{8,8},{8,9},{7,10}};
	std::vector<ls::Position> food = {{0,0},{10,0},{1,1},{3,1},{0,2},{0,4},{0,5},{0,6},{0,7},{2,7},{0,8},{4,8},{5,9},{9,9},{0,10},{4,10},{6,10},{9,10},{10,0}};
	auto s1 = ls::Snake(std::move(snake1), 98, ls::SnakeFlags::ByIndex(0));
	auto s2 = ls::Snake(std::move(snake2), 80, ls::SnakeFlags::ByIndex(1));
	auto state = ls::State(11,11, {s1, s2}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isUp());
	CHECK(state.getSnake(1).getDirection().isRight());
	CHECK(state.getLivingSquads().size() == 2);
	
	const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
		std::map<ls::SnakeFlags, StateOfMind> mindmap;
		for (auto& squad : state.state.getLivingSquads())
			mindmap.insert({squad, StateOfMind(squad)});
		return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	};
	Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10});
	auto move = search.findBestMove(State(ls::gm::Standard, state), createEvaluatorCallback);
	CHECK(move.isLeft());
}

TEST_CASE("Scenario 1 -- swapped", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ o . . . o . o . . o o ║	Snake 2:	<22
	 * ║ . . . . . o . 2 2 o . ║	Food:		o
	 * ║ o . . . o . . . 2 . . ║	Empty:		.
	 * ║ o . o . . . . . 2 2 . ║
	 * ║ o . . . 1 1 1 . 2 2 . ║
	 * ║ o . . . 1 1 1 . 2 . . ║
	 * ║ o . . . 1 . . . 2 . . ║
	 * ║ . . . . 1 . . . 2 . . ║
	 * ║ o . . . 1 1 . . 2 2 > ║
	 * ║ . o . o 1 1 . . . ^ . ║
	 * ║ o . . . 1 1 1 1 1 1 o ║
	 * ╚═══════════════════════╝
	 */
	std::vector<ls::Position> snake1 = {{10,2},{9,2},{8,2},{8,3},{8,4},{8,5},{8,6},{9,6},{9,7},{8,7},{8,8},{8,9},{7,10}};
	std::vector<ls::Position> snake2 = {{9,1},{9,0},{8,0},{7,0},{6,0},{5,0},{4,0},{4,1},{5,1},{5,2},{4,2},{4,3},{4,4},{4,5},{4,6},{5,6},{6,6},{6,5},{5,5}};
	std::vector<ls::Position> food = {{0,0},{10,0},{1,1},{3,1},{0,2},{0,4},{0,5},{0,6},{0,7},{2,7},{0,8},{4,8},{5,9},{9,9},{0,10},{4,10},{6,10},{9,10},{10,0}};
	auto s1 = ls::Snake(std::move(snake1), 98, ls::SnakeFlags::ByIndex(0));
	auto s2 = ls::Snake(std::move(snake2), 80, ls::SnakeFlags::ByIndex(1));
	auto state = ls::State(11,11, {s1, s2}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isRight());
	CHECK(state.getSnake(1).getDirection().isUp());
	CHECK(state.getLivingSquads().size() == 2);
	
	const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
		std::map<ls::SnakeFlags, StateOfMind> mindmap;
		for (auto& squad : state.state.getLivingSquads())
			mindmap.insert({squad, StateOfMind(squad)});
		return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	};
	Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10});
	auto move = search.findBestMove(State(ls::gm::Standard, state), createEvaluatorCallback);
	CHECK(move.isUp());
}

TEST_CASE("Scenario 2", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . o . ^ . . . o o o ║	Snake 2:	<22
	 * ║ . 2 2 > 1 . . . . . o ║	Food:		o
	 * ║ 2 2 . . 1 . . . . . . ║	Empty:		.
	 * ║ 2 2 . . 1 . . . . . . ║
	 * ║ . 2 . . 1 1 . . . . . ║
	 * ║ . 2 . . . 1 . . . . . ║
	 * ║ . 2 . . . . . . . . . ║
	 * ║ 2 2 . . . . . . . . . ║
	 * ║ 2 . . . . . . . . o o ║
	 * ║ . . . . . . . . . . . ║
	 * ║ o . . . . . . . o . . ║
	 * ╚═══════════════════════╝
	 */
	std::vector<ls::Position> snake1 = {{4,10},{4,9},{4,8},{4,7},{4,6},{5,6},{5,5}};
	std::vector<ls::Position> snake2 = {{3,9},{2,9},{1,9},{1,8},{0,8},{0,7},{1,7},{1,6},{1,5},{1,4},{1,3},{0,3},{0,2}};
	std::vector<ls::Position> food = {{0,0},{0,8},{2,9},{2,10},{9,10},{10,2},{10,8},{10,9},{10,10}};
	auto s1 = ls::Snake(std::move(snake1), 98, ls::SnakeFlags::ByIndex(0));
	auto s2 = ls::Snake(std::move(snake2), 80, ls::SnakeFlags::ByIndex(1));
	auto state = ls::State(11,11, {s1, s2}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isUp());
	CHECK(state.getSnake(1).getDirection().isRight());
	CHECK(state.getLivingSquads().size() == 2);
	
	const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
		std::map<ls::SnakeFlags, StateOfMind> mindmap;
		for (auto& squad : state.state.getLivingSquads())
			mindmap.insert({squad, StateOfMind(squad)});
		return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	};
	Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10});
	auto move = search.findBestMove(State(ls::gm::Standard, state), createEvaluatorCallback);
	CHECK(move.isRight());
}

TEST_CASE("Scenario 2 -- swapped", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . o . ^ . . . o o o ║	Snake 2:	<22
	 * ║ . 2 2 > 1 . . . . . o ║	Food:		o
	 * ║ 2 2 . . 1 . . . . . . ║	Empty:		.
	 * ║ 2 2 . . 1 . . . . . . ║
	 * ║ . 2 . . 1 1 . . . . . ║
	 * ║ . 2 . . . 1 . . . . . ║
	 * ║ . 2 . . . . . . . . . ║
	 * ║ 2 2 . . . . . . . . . ║
	 * ║ 2 . . . . . . . . o o ║
	 * ║ . . . . . . . . . . . ║
	 * ║ o . . . . . . . o . . ║
	 * ╚═══════════════════════╝
	 */
	std::vector<ls::Position> snake1 = {{3,9},{2,9},{1,9},{1,8},{0,8},{0,7},{1,7},{1,6},{1,5},{1,4},{1,3},{0,3},{0,2}};
	std::vector<ls::Position> snake2 = {{4,10},{4,9},{4,8},{4,7},{4,6},{5,6},{5,5}};
	std::vector<ls::Position> food = {{0,0},{0,8},{2,9},{2,10},{9,10},{10,2},{10,8},{10,9},{10,10}};
	auto s1 = ls::Snake(std::move(snake1), 98, ls::SnakeFlags::ByIndex(0));
	auto s2 = ls::Snake(std::move(snake2), 80, ls::SnakeFlags::ByIndex(1));
	auto state = ls::State(11,11, {s1, s2}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isRight());
	CHECK(state.getSnake(1).getDirection().isUp());
	CHECK(state.getLivingSquads().size() == 2);
	
	const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
		std::map<ls::SnakeFlags, StateOfMind> mindmap;
		for (auto& squad : state.state.getLivingSquads())
			mindmap.insert({squad, StateOfMind(squad)});
		return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	};
	Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10});
	auto move = search.findBestMove(State(ls::gm::Standard, state), createEvaluatorCallback);
	CHECK(move.isDown());
}

TEST_CASE("Scenario 3", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . . . . . . . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Food:		o
	 * ║ . . . . . . . . . . . ║	Empty:		.
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ╚═══════════════════════╝
	 */
	
}

TEST_CASE("Scenario 4", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . . . . . . . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Food:		o
	 * ║ . . . . . . . . . . . ║	Empty:		.
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ╚═══════════════════════╝
	 */
	
}

TEST_CASE("Scenario 5", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . . . . . . . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Food:		o
	 * ║ . . . . . . . . . . . ║	Empty:		.
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ╚═══════════════════════╝
	 */
	
}

TEST_CASE("Scenario 6", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . . . . . . . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Food:		o
	 * ║ . . . . . . . . . . . ║	Empty:		.
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ╚═══════════════════════╝
	 */
	
}

TEST_CASE("Scenario 7", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . o o o . . . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Food:		o
	 * ║ . . . . . . . . . . . ║	Empty:		.
	 * ║ . . . . . . . . o . . ║
	 * ║ o . . . . . . . 1 1 > ║
	 * ║ . . . . 2 2 2 > o . . ║
	 * ║ . . . . 2 . . . . . o ║
	 * ║ . . . o . . . . . . . ║
	 * ║ . . . . . . . . . o o ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ╚═══════════════════════╝
	 */
	//Bin gerade nicht mehr sicher, ob eine Bewegung nach oben tatsächlich das beste für 1 ist.
	//std::vector<ls::Position> snake1 = {{10,6},{9,6},{8,6}};
	//std::vector<ls::Position> snake2 = {/*TODO*/};
	//std::vector<ls::Position> food = {/*TODO*/};
	//auto s1 = ls::Snake(std::move(snake1), 98, ls::SnakeFlags::ByIndex(0));
	//auto s2 = ls::Snake(std::move(snake2), 80, ls::SnakeFlags::ByIndex(1));
	//auto state = ls::State(11,11, {s1, s2}, std::move(food));
	//
	//CHECK(state.getSnake(0).getDirection().isRight());
	//CHECK(state.getSnake(1).getDirection().isRight());
	//CHECK(state.getLivingSquads().size() == 2);
	//
	//const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
	//	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	//	for (auto& squad : state.state.getLivingSquads())
	//		mindmap.insert({squad, StateOfMind(squad)});
	//	return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	//};
	//Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10});
	//auto move = search.findBestMove(State(ls::gm::Standard, state), createEvaluatorCallback);
	//CHECK(move.isUp());
}

TEST_CASE("Scenario 8", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . . . . . . . . ║	Snake 2:	<22
	 * ║ . . . . o . . . . . . ║	Food:		o
	 * ║ . . . . o . . . . . . ║	Empty:		.
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . . . . . . . . 1 ║
	 * ║ . . o . . . . . 2 2 1 ║
	 * ║ . . . . . . 2 2 2 2 1 ║
	 * ║ . . . . . . v . . 2 1 ║
	 * ║ . . . . . . . . . < 1 ║
	 * ╚═══════════════════════╝
	 */
	std::vector<ls::Position> snake1 = {{9,0},{10,0},{10,1},{10,2},{10,3},{10,4}};
	std::vector<ls::Position> snake2 = {{6,1},{6,2},{7,2},{8,2},{8,3},{9,3},{9,2},{9,1}};
	std::vector<ls::Position> food = {{2,3},{4,8},{4,9}};
	auto s1 = ls::Snake(std::move(snake1), 80, ls::SnakeFlags::ByIndex(0));
	auto s2 = ls::Snake(std::move(snake2), 98, ls::SnakeFlags::ByIndex(1));
	auto state = ls::State(11,11, {s1, s2}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isLeft());
	CHECK(state.getSnake(1).getDirection().isDown());
	CHECK(state.getLivingSquads().size() == 2);
	CHECK((ls::gm::Standard.getUnblockedActions(state, 0)==(ls::Move::left | ls::Move::up)));
	
	const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
		std::map<ls::SnakeFlags, StateOfMind> mindmap;
		for (auto& squad : state.state.getLivingSquads())
			mindmap.insert({squad, StateOfMind(squad)});
		return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	};
	Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10});
	auto move = search.findBestMove(State(ls::gm::Standard, state), createEvaluatorCallback);
	CHECK((ls::gm::Standard.getUnblockedActions(state, 0) == ls::Move::left | ls::Move::up));
	CHECK(move.isUp());
}

TEST_CASE("Scenario 9", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . o . . . . . . . o ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Snake 3:	<33
	 * ║ . . . . . . . . . . o ║	Snake 4:	<44
	 * ║ . . . . . . . . . . . ║	Food:		o
	 * ║ . . . . . . . . . . 4 ║	Empty:		.
	 * ║ . . 2 . . . . . < 4 4 ║
	 * ║ o . 2 . . . . . . . . ║
	 * ║ . . 2 . . . . . . . . ║
	 * ║ . < 2 . . . . . . . . ║
	 * ║ < 3 3 3 . . . . . . . ║
	 * ║ o < 1 1 1 1 . . . . . ║
	 * ╚═══════════════════════╝
	 * Squad 1: snake 1, 2
	 * Squad 2: snake 3, 4
	 */
	auto gamemode = ls::gm::SquadGamemode();
	gamemode.setAllowBodyCollisions(true);
	gamemode.setSharedElimination(true);
	gamemode.setSharedHealth(true);
	gamemode.setSharedLength(true);
	constexpr auto infty = std::numeric_limits<float>::infinity();
	std::vector<ls::Position> snake1 = {{1,0},{2,0},{3,0},{4,0},{5,0}};
	std::vector<ls::Position> snake2 = {{1,2},{2,2},{2,3},{2,4},{2,5}};
	std::vector<ls::Position> snake3 = {{0,1},{1,1},{2,1},{3,1}};
	std::vector<ls::Position> snake4 = {{8,5},{9,5},{10,5},{10,6}};
	std::vector<ls::Position> food = {{2,3},{4,8},{4,9}};
	auto s1 = ls::Snake(std::move(snake1), 90, ls::SnakeFlags::Player1 | ls::SnakeFlags::Player2);
	auto s2 = ls::Snake(std::move(snake2), 90, ls::SnakeFlags::Player1 | ls::SnakeFlags::Player2);
	auto s3 = ls::Snake(std::move(snake3), 90, ls::SnakeFlags::Player3 | ls::SnakeFlags::Player4);
	auto s4 = ls::Snake(std::move(snake4), 90, ls::SnakeFlags::Player3 | ls::SnakeFlags::Player4);
	auto state = ls::State(11,11, {s1, s2, s3, s4}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isLeft());
	CHECK(state.getSnake(1).getDirection().isLeft());
	CHECK(state.getSnake(2).getDirection().isLeft());
	CHECK(state.getSnake(3).getDirection().isLeft());
	CHECK(state.getNumSnakes() == 4);
	CHECK(state.getLivingSquads().size() == 2);
	
	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	for (auto& squad : state.getLivingSquads())
		mindmap.insert({squad, StateOfMind(squad)});
	Evaluator eval(gamemode, state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	
	Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10});
	auto score = search.minimax(State(gamemode, state), 8, -infty, infty, eval);
	CHECK(score >= 1000); //This is a winning state

	{
		auto nstate = State(gamemode, state).afterMove(ls::Move::left)
				.afterMove(ls::Move::left)
				.afterMove(ls::Move::left)
				.afterMove(ls::Move::left);
		CHECK(nstate.isGameOver());
		auto scores = eval.evaluateAll(nstate, 4);

		CHECK(scores[ls::SnakeFlags::Player1 | ls::SnakeFlags::Player2] >= 1000);
		CHECK(scores[ls::SnakeFlags::Player3 | ls::SnakeFlags::Player4] <= 1000);
	}
}

TEST_CASE("Scenario 10", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . o . . . . . . . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Snake 3:	<33
	 * ║ . . . . . . . o . . . ║	Snake 4:	<44
	 * ║ . . . . . . . . . . . ║	Food:		o
	 * ║ . . . . . 1 1 > . 3 . ║	Empty:		.
	 * ║ . . . . . 1 1 1 ^ 3 3 ║
	 * ║ . . . ^ . . . . 3 3 3 ║
	 * ║ . . . 2 2 . . . 3 3 . ║
	 * ║ . . . . 2 2 2 2 . . . ║
	 * ║ . . . . . . . 2 2 . . ║
	 * ║ . . o . o . . . . . . ║
	 * ╚═══════════════════════╝
	 */
	const auto& gamemode = ls::gm::Standard;
	constexpr auto infty = std::numeric_limits<float>::infinity();
	std::vector<ls::Position> snake1 = {{7,6},{6,6},{5,6},{5,5},{6,5},{7,5}};
	std::vector<ls::Position> snake2 = {{8,5},{8,4},{8,3},{9,3},{9,4},{10,4},{10,5},{9,5},{9,6}};
	std::vector<ls::Position> snake3 = {{3,4},{3,3},{4,3},{4,2},{5,2},{6,2},{7,2},{7,1},{8,1}};
	std::vector<ls::Position> food = {{2,0},{4,0},{7,8},{2,10}};
	auto s1 = ls::Snake(std::move(snake1), 88, ls::SnakeFlags::Player1);
	auto s2 = ls::Snake(std::move(snake2), 75, ls::SnakeFlags::Player2);
	auto s3 = ls::Snake(std::move(snake3), 98, ls::SnakeFlags::Player3);
	auto state = ls::State(11,11, {s1, s2, s3}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isRight());
	CHECK(state.getSnake(1).getDirection().isUp());
	CHECK(state.getSnake(2).getDirection().isUp());
	CHECK(state.getNumSnakes() == 3);
	CHECK(state.getLivingSquads().size() == 3);
	
	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	for (auto& squad : state.getLivingSquads())
		mindmap.insert({squad, StateOfMind(squad)});
	Evaluator eval(gamemode, state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);

	using Search = Search<State, ls::Move, Evaluator, ls::SnakeFlags>;
	auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::right), 3, eval);
	CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving right is a losing move
}

TEST_CASE("Scenario 11", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . . . . . o . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Snake 3:	<33
	 * ║ . . ^ 4 . . . . . . . ║	Snake 4:	<44
	 * ║ . . 4 4 . . . . . . . ║	Food:		o
	 * ║ . . . . . . 2 2 . 1 1 ║	Empty:		.
	 * ║ . . . . . o 2 > . v . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . 3 3 > . . . . . . . ║
	 * ║ . 3 . . . . . . . . . ║
	 * ║ . 3 . . . . . . . . . ║
	 * ║ . . . . . . . . o . . ║
	 * ╚═══════════════════════╝
	 */
	const auto& gamemode = ls::gm::Standard;
	constexpr auto infty = std::numeric_limits<float>::infinity();
	std::vector<ls::Position> snake1 = {{9,5},{9,6},{10,6}};
	std::vector<ls::Position> snake2 = {{7,5},{6,5},{6,6},{7,6}};
	std::vector<ls::Position> snake3 = {{3,3},{2,3},{1,3},{1,2},{1,1}};
	std::vector<ls::Position> snake4 = {{2,8},{2,7},{3,7},{3,8}};
	std::vector<ls::Position> food = {{5,5},{8,10},{8,0}};
	auto s1 = ls::Snake(std::move(snake1), 92, ls::SnakeFlags::Player1);
	auto s2 = ls::Snake(std::move(snake2), 98, ls::SnakeFlags::Player2);
	auto s3 = ls::Snake(std::move(snake3), 99, ls::SnakeFlags::Player3);
	auto s4 = ls::Snake(std::move(snake4), 94, ls::SnakeFlags::Player4);
	auto state = ls::State(11,11, {s1, s2, s3, s4}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isDown());
	CHECK(state.getSnake(1).getDirection().isRight());
	CHECK(state.getSnake(2).getDirection().isRight());
	CHECK(state.getSnake(3).getDirection().isUp());
	CHECK(state.getNumSnakes() == 4);
	CHECK(state.getLivingSquads().size() == 4);
	
	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	for (auto& squad : state.getLivingSquads())
		mindmap.insert({squad, StateOfMind(squad)});
	Evaluator eval(gamemode, state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);

	using Search = Search<State, ls::Move, Evaluator, ls::SnakeFlags>;
	auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::left), 8, eval);
	CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving left is a losing move
}

TEST_CASE("Scenario 12", "[Scenario]") {
	/**
	 * ╔═══════════════════════╗	Snake 1:	<11
	 * ║ . . . . o . . . . . . ║	Snake 2:	<22
	 * ║ . . . . . . . . . . . ║	Snake 3:	<33
	 * ║ 3 3 . . 1 . < 2 . . . ║	Snake 4:	<44
	 * ║ 3 v . < 1 . . 2 . . . ║	Food:		o
	 * ║ 3 . . . . . . 2 . . . ║	Empty:		.
	 * ║ 3 . . . . 4 . . . . . ║
	 * ║ . . . . . 4 ^ . . . . ║
	 * ║ . . . . . 4 4 . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ║ . . . o . . . . . . . ║
	 * ║ . . . . . . . . . . . ║
	 * ╚═══════════════════════╝
	 */
	const auto& gamemode = ls::gm::Standard;
	std::vector<ls::Position> snake1 = {{3,7},{4,7},{4,8}};
	std::vector<ls::Position> snake2 = {{6,8},{7,8},{7,7},{7,6}};
	std::vector<ls::Position> snake3 = {{1,7},{1,8},{0,8},{0,7},{0,6},{0,5}};
	std::vector<ls::Position> snake4 = {{6,4},{6,3},{5,3},{5,4},{5,5}};
	std::vector<ls::Position> food = {{4,10},{3,1}};
	auto s1 = ls::Snake(std::move(snake1), 82, ls::SnakeFlags::Player1);
	auto s2 = ls::Snake(std::move(snake2), 84, ls::SnakeFlags::Player2);
	auto s3 = ls::Snake(std::move(snake3), 94, ls::SnakeFlags::Player3);
	auto s4 = ls::Snake(std::move(snake4), 95, ls::SnakeFlags::Player4);
	auto state = ls::State(11,11, {s1, s2, s3, s4}, std::move(food));
	
	CHECK(state.getSnake(0).getDirection().isLeft());
	CHECK(state.getSnake(1).getDirection().isLeft());
	CHECK(state.getSnake(2).getDirection().isDown());
	CHECK(state.getSnake(3).getDirection().isUp());
	CHECK(state.getNumSnakes() == 4);
	CHECK(state.getLivingSquads().size() == 4);
	
	const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
		std::map<ls::SnakeFlags, StateOfMind> mindmap;
		for (auto& squad : state.state.getLivingSquads())
			mindmap.insert({squad, StateOfMind(squad)});
		return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	};

	std::map<ls::SnakeFlags, StateOfMind> mindmap;
	for (auto& squad : state.getLivingSquads())
		mindmap.insert({squad, StateOfMind(squad)});
	Evaluator eval(gamemode, state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);

	using Search = Search<State, ls::Move, Evaluator, ls::SnakeFlags>;
	Search search({.initialDepth=10});
	auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::left), 8, eval);
	CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving left is a losing move

	CHECK_FALSE(search.findBestMove(State(gamemode, state), createEvaluatorCallback).isLeft());

	//Check for reordering
	{//243
		auto state = ls::State(11,11, {s1, s2, s4, s3}, std::move(food));
		auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::left), 8, eval);
		CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving left is a losing move
		CHECK_FALSE(search.findBestMove(State(gamemode, state), createEvaluatorCallback).isLeft());
	}{//324
		auto state = ls::State(11,11, {s1, s3, s2, s4}, std::move(food));
		auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::left), 8, eval);
		CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving left is a losing move
		CHECK_FALSE(search.findBestMove(State(gamemode, state), createEvaluatorCallback).isLeft());
	}{//342
		auto state = ls::State(11,11, {s1, s3, s4, s2}, std::move(food));
		auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::left), 8, eval);
		CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving left is a losing move
		CHECK_FALSE(search.findBestMove(State(gamemode, state), createEvaluatorCallback).isLeft());
	}{//423
		auto state = ls::State(11,11, {s1, s4, s2, s3}, std::move(food));
		auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::left), 8, eval);
		CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving left is a losing move
		CHECK_FALSE(search.findBestMove(State(gamemode, state), createEvaluatorCallback).isLeft());
	}{//432
		auto state = ls::State(11,11, {s1, s4, s3, s2}, std::move(food));
		auto score = Search::maxN(State(gamemode, state).afterMove(ls::Move::left), 8, eval);
		CHECK(score[ls::SnakeFlags::Player1] <= -1000); //Moving left is a losing move
		CHECK_FALSE(search.findBestMove(State(gamemode, state), createEvaluatorCallback).isLeft());
	}
}