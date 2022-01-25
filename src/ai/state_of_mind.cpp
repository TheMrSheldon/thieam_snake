#include <ai/state_of_mind.h>

#include <ai/eval.h>
#include <iostream>

StateOfMind::StateOfMind(ls::SnakeFlags squad) noexcept : squad(squad) {}

void StateOfMind::setTarget(StateOfMind::Target target) noexcept {
	this->target = target;
}


static inline float relEval(float player, float opponent) noexcept {
	if (player >= opponent)
		return player/(player + opponent);
	return -opponent/(player + opponent);
	//return player / (player + opponent);
}

float StateOfMind::getRating(const ls::State& state, Evaluation& eval) const noexcept {
	float depth_delta = 1-(1.0f / (eval.depth + 1));
	if (eval.winner != ls::SnakeFlags::None) {
		if (eval.winner.containsAll(squad)) {
			if (eval.winner == squad)
				return 1000 + depth_delta; //win (prefer early wins)
			return -750; //tie
		} else
			return -1000 - depth_delta; //loss (prefer late losses)
	}
	if (!state.getLivingSquads().contains(squad))
		return -1000 + depth_delta; //loss (prefer late losses)

	// Calculate minimum health
	const int squadWeight = 10;
	int squadCount = state.getLivingSquads().size();
	int minSquadHealth = 100;
	int minOpponentHealth = 100;
	unsigned totSquadControl = 0;
	unsigned totOpponentControl = 0;
	for (unsigned snake = 0; snake < state.getNumSnakes(); ++snake) {
		if (squad.containsAll(ls::SnakeFlags::ByIndex(snake))) {
			minSquadHealth = std::min(minSquadHealth, state.getSnake(snake).getHealth());
			totSquadControl += eval.snakes[snake].mobility;
		} else {
			minOpponentHealth = std::min(minOpponentHealth, state.getSnake(snake).getHealth());
			totOpponentControl += eval.snakes[snake].mobility;
		}
	}
	return .2f*relEval((float)minSquadHealth, (float)minOpponentHealth)
		+ 5*relEval((float)totSquadControl, (float)totOpponentControl)
		- squadCount * squadWeight;
}