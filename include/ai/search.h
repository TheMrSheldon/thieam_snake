#pragma once

#include <functional>
#include <limits>
#include <map>
#include <type_traits>

struct SearchSettings {
	unsigned initialDepth;
	unsigned timeout;
	unsigned numThreads;
};

//https://stackoverflow.com/questions/53673442/simplest-way-to-determine-return-type-of-function
template<typename Callable>
using return_type_of = typename decltype(std::function{std::declval<Callable>()})::result_type;

//TODO: implement timeouts
//TODO: implement iterative deepening
template<typename State, typename Move, typename Evaluator, typename Party>
class Search final {
private:
	const SearchSettings settings;

	float iterativeDeepening(const State& state, Evaluator& evaluator) const {
		return -minimax(state, settings.initialDepth-1, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), evaluator);
	}
	static float minimax(const State& state, unsigned depth, float alpha, float beta, Evaluator& evaluator) {
		if (state.isGameOver() || depth==0)
			return evaluator.evaluate(state, depth);//FIXME: this pass the actual depth instead of the remaining one
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

	static std::map<Party, float> maxN(const State& state, unsigned depth, Evaluator& evaluator) {
		if (state.isGameOver() || depth==0)
			return std::move(evaluator.evaluateAll(state, depth));//FIXME: this pass the actual depth instead of the remaining one
		const auto party = state.getCurrentParty();
		std::map<Party, float> best;
		for (const auto& action : state.getValidActions()) {
			auto next = state.afterMove(action);
			auto scores = std::move(maxN(next, depth-1, evaluator));
			if (best.empty() || best[party] < scores[party])
				best = std::move(scores);
		}
		return best;
	}

public:
	Search(SearchSettings settings = {}) noexcept : settings(settings) {}

	template<typename C>
	Move findBestMove(const State& state, C createEvaluator) const {
		Evaluator eval = std::move(createEvaluator(state));

		if (state.getNumParties() == 2) {
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
		} else {
			const auto party = state.getCurrentParty();
			return maxN(state, settings.initialDepth, eval)[party];
		}
	}
};