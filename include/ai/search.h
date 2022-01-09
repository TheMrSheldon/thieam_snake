#pragma once

#include <coroutine>

#include "../utils/generator.h"

template<typename T>
concept IsState = requires(const T& t){
	t.getGamemode();
	t.getNumPlayers();
	t.getWidth();
	t.getHeight();

	t.getValidActions().begin();
	t.getValidActions().end();

	t.afterMove(*t.getValidActions().begin());
	t.getTurn();
};

struct SearchSettings {
	unsigned initialDepth;
	/**
	 * @brief The max. allowed search time in ms. Please note that the actual routine call may take
	 * a bit longer than the specified time due to overhead of terminating the search.
	 */
	unsigned timeout;
	unsigned numThreads;
};

template<typename State, typename Move, typename Evaluator>
requires IsState<State>
class Search final {
#ifdef TESTING
public:
#else
private:
#endif
	const SearchSettings settings;

	static Generator<float> iterativeDeepening(const State& state, unsigned startDepth, Evaluator& evaluator, unsigned increment=1) {
		for (unsigned depth = startDepth; ; depth += increment)
			co_yield -minimax(state, depth, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), evaluator);
	}
	static float minimax(const State& state, unsigned depth, float alpha, float beta, Evaluator& evaluator) {
		if (state.isGameOver() || depth==0)
			return evaluator.evaluate(state);
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			float score;
			if ((state.getTurn() == 0) != (next.getTurn() == 0))
				score = -minimax(next, depth-1, -beta, -alpha, evaluator);
			else
				score = minimax(next, depth-1, alpha, beta, evaluator);
			if (score > alpha) {
				alpha = score;
				if (alpha >= beta)
					break;
			}
		}
		return alpha;
	}

public:
	Search(SearchSettings settings = {}) noexcept : settings(settings) {}
	Move findBestMove(const State& state) const {
		/*Evaluator eval(state.getGamemode(), state.getNumPlayers(), state.getWidth(), state.getHeight());
		float best_score = -std::numeric_limits<float>::infinity();
		Move best;
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			auto score = Search::iterativeDeepening(next, settings.initialDepth-1, eval)();
			if (score >= best_score) {
				best = action;
				best_score = score;
			}
		}
		return best;*/

		Evaluator eval(state.getGamemode(), state.getNumPlayers(), state.getWidth(), state.getHeight());
		std::vector<std::pair<Move, Generator<float>>> routines;
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			routines.emplace_back(std::make_pair(action, Search::iterativeDeepening(next, settings.initialDepth-1, eval)));
		}

		Move lastResult;
		//while (true) {
		//for (int i = 0; i < 2; i++) {
			float bestScore = -std::numeric_limits<float>::infinity();
			Move best;
			for (auto& entry : routines) {
				auto& action = std::get<0>(entry);
				auto& routine = std::get<1>(entry);
				float score = routine();
				if (score >= bestScore) {
					best = action;
					bestScore = score;
				}
			}
			lastResult = best;
		//}
		return lastResult;
	}
};