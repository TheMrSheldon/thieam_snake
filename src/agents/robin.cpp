#include "agents/robin.h"

#include "ai/state.h"
#include "ai/search.h"
#include "ai/eval.h"

#include <iostream>

using namespace agents;

Robin::Robin() noexcept : Agent("THIeam", "#8b00ff", "smart-caterpillar", "mouse", "0.01a") {}

void Robin::startGame(const GameInfo& info) {}

ls::Move Robin::getAction(const GameInfo& info, uint32_t turn, ls::State& state) {
	Search<State, ls::Move, Evaluator> search({.initialDepth = 10, .timeout = info.timeout-50});
	const auto gamemode = info.constructGamemode();
	return search.findBestMove(State(*gamemode, state));
}

void Robin::endGame(const GameInfo& info) {
	
}