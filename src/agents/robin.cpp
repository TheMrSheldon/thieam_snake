#include "agents/robin.h"

#include "ai/state.h"
#include "ai/state_of_mind.h"
#include "ai/search.h"
#include "ai/eval.h"

#include <iostream>

using namespace agents;

Robin::Robin() noexcept : Agent("THIeam", "#8b00ff", "smart-caterpillar", "mouse", "0.01a") {}

void Robin::startGame(const GameInfo& info) {}

ls::Move Robin::getAction(const GameInfo& info, uint32_t turn, ls::State& state) {
	StateOfMind mind;
	const auto createEvaluatorCallback = [mind](const State& state) -> Evaluator {
		return Evaluator(state.getGamemode(), state.getNumPlayers(), state.getWidth(), state.getHeight(), mind)
	};
	Search<State, ls::Move, Evaluator> search({.initialDepth = 10, .timeout = info.timeout-50});
	const auto gamemode = info.constructGamemode();
	return search.findBestMove(State(*gamemode, state), createEvaluatorCallback);
}

void Robin::endGame(const GameInfo& info) {
	
}