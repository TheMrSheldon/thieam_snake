#pragma once

#include <limits>

struct SearchSettings {
	unsigned initialDepth;
	unsigned timeout;
	unsigned numThreads;
};

//TODO: implement timeouts
//TODO: implement iterative deepening
template<typename State, typename Move, typename Evaluator>
class Search final {
private:
	const SearchSettings settings;

	float iterativeDeepening(const State& state, Evaluator& evaluator) const {
		return -minimax(state, settings.initialDepth-1, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), evaluator);
	}
	static float minimax(const State& state, unsigned depth, float alpha, float beta, Evaluator& evaluator) {
		if (state.isGameOver() || depth==0)
			return evaluator.evaluate(state, depth);
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

	template<typename C>
	Move findBestMove(const State& state, C createEvaluator) const {
		Evaluator eval = std::move(createEvaluator(state));

		float best_score = -std::numeric_limits<float>::infinity();
		Move best;
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			auto score = iterativeDeepening(next, eval);
			if (score >= best_score) {
				best = action;
				best_score = score;
			}
		}
		return best;
	}
};