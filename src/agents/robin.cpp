#include "agents/robin.h"

#include "ai/state.h"
#include "ai/state_of_mind.h"
#include "ai/search.h"
#include "ai/eval.h"

#include <iostream>

using namespace agents;

Robin::Robin() noexcept : Agent("THIeam", "#8b00ff", "smart-caterpillar", "mouse", "0.9a") {}

void Robin::startGame(const GameInfo& info) {}

ls::Move Robin::getAction(const GameInfo& info, uint32_t turn, ls::State& state) {
	//
	//Determine snake's mind state before applying the search to prevent different heuristics from being compared to each other
	//constexpr auto LengthThreshold = 3;
	//if(state.getSnake(0).length() <= LengthThreshold || state.getSnake(0).getHealth() <= 20)
	//	mind.setTarget(StateOfMind::Target::Grow);
	//else
	//	mind.setTarget(StateOfMind::Target::Control);
	//
	const auto createEvaluatorCallback = [](const State& state) -> Evaluator {
		std::map<ls::SnakeFlags, StateOfMind> mindmap;
		for (auto& squad : state.state.getLivingSquads())
			mindmap.insert({squad, StateOfMind(squad)});
		return Evaluator(state.getGamemode(), state.state.getNumSnakes(), state.getWidth(), state.getHeight(), mindmap);
	};
	Search<State, ls::Move, Evaluator, ls::SnakeFlags> search({.initialDepth = 10, .timeout = info.timeout-50});
	const auto gamemode = info.constructGamemode();
	return search.findBestMove(State(*gamemode, state), createEvaluatorCallback);
}

void Robin::endGame(const GameInfo& info) {
	
}