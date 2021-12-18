#include <ai/eval.h>

#include <libsnake/gamemodes/arena.h>

#include <algorithm>
#include <deque>

Evaluator::Evaluator(const ls::Gamemode& gamemode, unsigned width, unsigned height) noexcept : gamemode(gamemode), envbuffer(width, height) {}

Evaluation Evaluator::evaluate(const ls::State& state) noexcept {
	Evaluation result;
	result.winner = gamemode.getWinner(state);
	if (!gamemode.isGameOver(state)) {
		result.snakes = std::vector<SnakeEval>();
		for (size_t snake = 0; snake < state.getSnakes().size(); ++snake) {
			result.snakes.emplace_back(SnakeEval{
				.health = (unsigned)state.getSnake(snake).getHealth(),
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
		unsigned dist;
		size_t snake;
	};
	// Setup the envbuffer and frontier-queue
	envbuffer.clear();
	std::vector<size_t> foodReached(state.getSnakes().size(), 0);
	std::deque<PosStr> frontier;
	for (size_t snake = 0; snake < state.getSnakes().size(); ++snake) {
		if (!state.getSnake(snake).isDead()) {
			envbuffer.storeSnake(snake, state.getSnake(snake));
			frontier.emplace_back(state.getSnake(snake).getHeadPos(), snake, 0);
		}
	}
	// Scan the environment
	while (!frontier.empty()) {
		auto cur = frontier.front();
		frontier.pop_front();
		for (const auto dir : {ls::Position(-1,0), ls::Position(1,0), ls::Position(0,-1), ls::Position(0,1)}) {
			auto pos = cur.pos + dir;
			if (!envbuffer.isBlockedAtTurn(pos, cur.snake, cur.dist+1)) {
				envbuffer.blockAfterTurn(pos, cur.snake, cur.dist+1);
				frontier.emplace_back(pos, cur.snake, cur.dist+1);
				foodReached[cur.snake] += state.isFoodAt(pos);
			}
		}
	}
	// Store the results in the Evaluation-datastructur
	for (size_t snake = 0; snake < state.getSnakes().size(); ++snake) {
		results.snakes[snake].mobility = (unsigned)envbuffer.getAreaControl(snake);
		results.snakes[snake].mobility_per_area = envbuffer.getAreaControl(snake)/(float)(state.getWidth()*state.getHeight());
		results.snakes[snake].foodInReach = (unsigned)foodReached[snake];
	}
}

static inline float relEval(float player, float opponent) noexcept {
	if (player >= opponent)
		return player/(player + opponent);
	return -opponent/(player + opponent);
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
	return .2f*relEval((float)eval.snakes[0].health, (float)eval.snakes[1].health)
		+ 5*relEval((float)eval.snakes[0].mobility, (float)eval.snakes[1].mobility)
		+ .05f*relEval((float)eval.snakes[0].foodInReach, (float)eval.snakes[1].foodInReach)
		+ 3*relEval((float)eval.snakes[0].choice, (float)eval.snakes[1].choice);
}