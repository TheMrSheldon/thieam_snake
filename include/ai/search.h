#pragma once

struct SearchSettings {
	unsigned initialDepth;
	unsigned timeout;
	unsigned numThreads;
};

enum class StateOfMind { Grow, Control };

template<typename State, typename Move, typename Evaluator>
class Search final {
private:
	const SearchSettings settings;

	float iterativeDeepening(const State& state, Evaluator& evaluator) const {
		return -minimax(state, settings.initialDepth-1, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), evaluator);
	}
	static float minimax(const State& state, unsigned depth, float alpha, float beta, Evaluator& evaluator, MindState mindState) {
		if (state.isGameOver() || depth==0)
			return evaluator.evaluate(state, depth, mindState);
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			float score;
			if ((state.getTurn() == 0) != (next.getTurn() == 0))
				score = -minimax(next, depth-1, -beta, -alpha, evaluator, mindState);
			else
				score = minimax(next, depth-1, alpha, beta, evaluator, mindState);
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
		Evaluator eval(state.getGamemode(), state.getNumPlayers(), state.getWidth(), state.getHeight());
		
		//Determine snake's mind state before applying the minimax to prevent different heuristics from being compared to each other
		StateOfMind mindState;
		constexpr auto LengthThreshold = 3;
		if(state.state.getSnake(0).length() <= LengthThreshold && eval.snakes[0].foodInReach > 0)
			mindState = StateOfMind.Grow;
		else
			mindState = StateOfMind.Control;

		float best_score = -std::numeric_limits<float>::infinity();
		Move best;
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			auto score = iterativeDeepening(next, eval, mindState);
			if (score >= best_score) {
				best = action;
				best_score = score;
			}
		}
		return best;
	}
};