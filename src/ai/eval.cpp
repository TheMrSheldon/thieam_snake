#include <ai/eval.h>

#include <libsnake/gamemodes/standard.h>

#include <algorithm>
#include <deque>

Evaluator::Evaluator(const ls::Gamemode& gamemode, unsigned numSnakes, unsigned width, unsigned height, const std::map<ls::SnakeFlags, StateOfMind>& mind) noexcept
	: gamemode(gamemode),  mind(mind), envbuffer(numSnakes, width, height) {}

Evaluator::Evaluator(Evaluator&& other) noexcept : gamemode(other.gamemode), mind(other.mind), envbuffer(std::move(other.envbuffer)) {}

Evaluation Evaluator::evaluate(const ls::State& state, unsigned depth) noexcept {
	Evaluation result;
	result.winner = gamemode.getWinner(state);
	if (!gamemode.isGameOver(state)) {
		result.snakes = std::vector<SnakeEval>();
		for (size_t snake = 0; snake < state.getNumSnakes(); ++snake) {
			result.snakes.emplace_back(SnakeEval{
				.choice = gamemode.getUnblockedActions(state, 0).size()
			});
		}
		scanProximity(state, depth, result);
	}
	return result;
}

UNROLL
void Evaluator::scanProximity(const ls::State& state, unsigned depth, Evaluation& results) noexcept {
	struct PosStr {
		ls::Position pos;
		size_t snake;
		unsigned dist;
	};
	// Setup the envbuffer and frontier-queue
	envbuffer.clear();
	std::vector<size_t> foodReached(state.getNumSnakes(), 0);
	std::deque<PosStr> frontier;
	for (size_t snake = 0; snake < state.getNumSnakes(); ++snake) {
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
	for (size_t snake = 0; snake < state.getNumSnakes(); ++snake) {
		results.snakes[snake].mobility = (unsigned)envbuffer.getAreaControl(snake);
		//results.snakes[snake].bordercontrol = (unsigned)envbuffer.getBorderControl(snake);
		results.snakes[snake].mobility_per_area = envbuffer.getAreaControl(snake)/(float)(state.getWidth()*state.getHeight());
		results.snakes[snake].foodInReach = (unsigned)foodReached[snake];
	}
	results.depth = depth;
}

std::map<ls::SnakeFlags, float> Evaluator::evaluateAll(const State& state, unsigned depth) noexcept {
	auto eval = evaluate(state.state, depth);
	std::map<ls::SnakeFlags, float> ret;
	for (auto& entry : mind)
		ret[entry.first] = entry.second.getRating(state.state, eval);
	return std::move(ret);
}

float Evaluator::evaluate(const State& state, unsigned depth) noexcept {
    auto eval = evaluate(state.state, depth);
	auto minParty = state.state.getSnake(0).getSquad();
	return mind.find(minParty)->second.getRating(state.state, eval);
}