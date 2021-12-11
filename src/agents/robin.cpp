#include "agents/robin.h"

#include "ai/state.h"
#include "ai/search.h"
#include "ai/eval.h"

#include <iostream>

using namespace agents;

Robin::Robin() noexcept : Agent("THIeam", "#8b00ff", "smart-caterpillar", "mouse", "0.01a") {}

void Robin::startGame(const GameInfo& info) {
	
}

Move Robin::getAction(const GameInfo& info, uint32_t turn, ls::State& state) {
	std::cout << "Start search" << std::endl;
	Search<State, Move, Evaluator> search({.initialDepth = 2});
	return search.findBestMove(State(state));
}

void Robin::endGame(const GameInfo& info) {
	
}