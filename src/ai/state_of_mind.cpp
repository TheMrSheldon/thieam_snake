#include <ai/state_of_mind.h>

#include <ai/eval.h>

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
    //TODO: implement properly -- this is only a fixed basic 2-player evaluation
	float depth_delta = 1.0f / (eval.depth + 1);
	if (eval.winner != ls::SnakeFlags::None) {
		if (eval.winner.containsAll(ls::SnakeFlags::Player1 | ls::SnakeFlags::Player2))
			return -50;
		else if (eval.winner.containsAny(ls::SnakeFlags::Player1))
			return 100 + depth_delta;// + depth;
		return -100 + depth_delta;// - depth;
	}

    return .2f*relEval((float)state.getSnake(0).getHealth(), (float)state.getSnake(1).getHealth())
		+ 5*relEval((float)eval.snakes[0].mobility, (float)eval.snakes[1].mobility);
		//+ .05f*relEval((float)eval.snakes[0].foodInReach, (float)eval.snakes[1].foodInReach)
		//+ 3*relEval((float)eval.snakes[0].choice, (float)eval.snakes[1].choice);

    //This was copied over from the former evaluation
	/*auto robin =  state.state.getSnake(0);
	auto mobilityScore = relEval((float)eval.snakes[0].mobility, (float)eval.snakes[1].mobility);
	int our_length = robin.length();

	constexpr auto InitialFoodReward = 8;	//Feeding the first piece of food in endgame mode awards 2^-6 times this value's points
	constexpr auto FoodRewardDecay = 2;
	
	switch(mindState){
		case StateOfMind::Grow:
			return our_length + (100 - robin.getHealth())/100 + mobilityScore/1000;
		default:
			auto length_penalty = InitialFoodReward * std::powf(FoodRewardDecay, -our_length);
			return  - length_penalty;
	}*/
}