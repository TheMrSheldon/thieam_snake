#pragma once

struct SearchSettings {
	unsigned initialDepth;
	unsigned timeout;
	unsigned numThreads;
};

template<typename State, typename Move, typename Evaluator>
class Search final {
private:
	const SearchSettings settings;

	float iterativeDeepening(const State& state, Evaluator& evaluator) const {
		return -minimax(state, settings.initialDepth-1, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), evaluator);
	}
	static float minimax(const State& state, unsigned depth, float alpha, float beta, Evaluator& evaluator) {
		if (state.isGameOver() || depth==0)
			return evaluator.evaluate(state);
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			auto score = -minimax(next, depth-1, -beta, -alpha, evaluator);
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
		Evaluator eval;
		float best_score = -std::numeric_limits<float>::infinity();
		Move best;
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			auto score = iterativeDeepening(next, eval);
			printf("[%i] -> %f\n", action, score);
			if (score >= best_score) {
				best = action;
				best_score = score;
			}
		}
		return best;
	}
};