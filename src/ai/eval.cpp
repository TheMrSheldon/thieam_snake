#include <ai/eval.h>

#include <libsnake/gamemodes/arena.h>

#include <algorithm>
#include <deque>

Evaluator::Evaluator(const ls::Gamemode& gamemode, unsigned numSnakes, unsigned width, unsigned height) noexcept
	: gamemode(gamemode), envbuffer(numSnakes, width, height) {}

Evaluation Evaluator::evaluate(const ls::State& state) noexcept {
	Evaluation result;
	result.winner = gamemode.getWinner(state);
	if (!gamemode.isGameOver(state)) {
		result.snakes = std::vector<SnakeEval>();
		for (size_t snake = 0; snake < state.getSnakes().size(); ++snake) {
			result.snakes.emplace_back(SnakeEval{
				.choice = gamemode.getUnblockedActions(state, 0).size()
			});
		}
		scanProximity(state, result);
	}
	return result;
}

UNROLL
void Evaluator::scanProximity(const ls::State& state, Evaluation& results) noexcept {
	struct PosStr {
		ls::Position pos;
		size_t snake;
		unsigned dist;
	};
	// Setup the envbuffer and frontier-queue
	envbuffer.clear();
	std::vector<size_t> foodReached(state.getSnakes().size(), 0);
	std::deque<PosStr> frontier;
	for (size_t snake = 0; snake < state.getSnakes().size(); ++snake) {
		if (!state.getSnake(snake).isDead()) {
			envbuffer.storeSnake(snake, state.getSnake(snake));
			envbuffer.blockAfterTurn(state.getSnake(snake).getHeadPos(), snake, 0);
			frontier.emplace_back(state.getSnake(snake).getHeadPos(), snake, 0);
		}
	}
	// Scan the environment
	while (!frontier.empty()) {
		auto cur = frontier.front();
		frontier.pop_front();
		for (const auto dir : {ls::Position(-1,0), ls::Position(1,0), ls::Position(0,-1), ls::Position(0,1)}) {
			auto pos = cur.pos + dir;
			if (!envbuffer.isBlockedAtTurn(state, pos, cur.snake, cur.dist+1)) {
				envbuffer.blockAfterTurn(pos, cur.snake, cur.dist+1);
				frontier.emplace_back(pos, cur.snake, cur.dist+1);
				foodReached[cur.snake] += state.isFoodAt(pos);
			}
		}
	}
	// Store the results in the Evaluation-datastructur
	for (size_t snake = 0; snake < state.getSnakes().size(); ++snake) {
		results.snakes[snake].mobility = (unsigned)envbuffer.getAreaControl(snake);
		//results.snakes[snake].bordercontrol = (unsigned)envbuffer.getBorderControl(snake);
		results.snakes[snake].mobility_per_area = envbuffer.getAreaControl(snake)/(float)(state.getWidth()*state.getHeight());
		results.snakes[snake].foodInReach = (unsigned)foodReached[snake];
	}
}

static inline float relEval(float player, float opponent) noexcept {
	/*if (player >= opponent)
		return player/(player + opponent);
	return -opponent/(player + opponent);*/
	return player/(player + opponent);
}

float Evaluator::evaluate(const State& state) noexcept {
	auto eval = evaluate(state.state);
	if (eval.winner != ls::SnakeFlags::None) {
		if (eval.winner.containsAll(ls::SnakeFlags::Player1 | ls::SnakeFlags::Player2))
			return -50;
		else if (eval.winner.containsAny(ls::SnakeFlags::Player1))
			return 100;
		return -100;
	}
	/*return .2f*relEval((float)state.state.getSnake(0).getHealth(), (float)state.state.getSnake(1).getHealth())
		+ 5*relEval((float)eval.snakes[0].mobility, (float)eval.snakes[1].mobility)
		+ .05f*relEval((float)eval.snakes[0].foodInReach, (float)eval.snakes[1].foodInReach)
		+ 3*relEval((float)eval.snakes[0].choice, (float)eval.snakes[1].choice);*/
	int our_length = state.state.getSnake(0).length();

	constexpr auto LengthThreshold = 5;
	constexpr auto InitialFoodReward = 4;	//Feeding the first piece of food in endgame mode awards 2^-6 times this value's points
	constexpr auto FoodRewardDecay = 2;
	if(our_length < LengthThreshold){
		return our_length - LengthThreshold - InitialFoodReward;	//Make sure that this returns at most the minimum endgameevaluation value
	} else {
		auto length_penalty = FoodRewardDecay * InitialFoodReward * std::powf(FoodRewardDecay, -our_length);
		return relEval((float)eval.snakes[0].mobility, (float)eval.snakes[1].mobility) - length_penalty;
	}
}