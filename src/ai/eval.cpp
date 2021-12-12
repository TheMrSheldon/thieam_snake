#include <ai/eval.h>

#include <libsnake/gamemodes/duel.h>

#include <algorithm>
#include <deque>

/*
 * The envbuffer stores some worldinformation relative to the snakes head:
 * ┌───────────┐ The X marks the snake's head whereas the dotted lines are
 * │     ┋     │ all of length [scan_radius]. As such it follows that the
 * │     ┋     │ envbuffer has size (2*scan_radius+1)².
 * │┉┉┉┉┉X┉┉┉┉┉│
 * │     ┋     │
 * │     ┋     │
 * └───────────┘
 */
Evaluator::Evaluator() noexcept : envbuffer(scan_area_width*scan_area_width) {}

Evaluation Evaluator::evaluate(const ls::State& state) noexcept {
    Evaluation result;
    result.winner = ls::gm::Duel.getWinner(state);
    if (!ls::gm::Duel.isGameOver(state)) {
        result.snakes[0].health = state.getSnake(0).getHealth();
        result.snakes[1].health = state.getSnake(1).getHealth();
        scanProximity(state, result);
    }
    return result;
}

bool Evaluator::isInEnvBuff(const Position& pos) const noexcept {
    return pos.x >= 0 && pos.y >= 0 && pos.x < scan_area_width && pos.y < scan_area_width;
}
uint8_t& Evaluator::getEnvBuffEntry(const Position& pos) noexcept {
    return envbuffer[pos.x + scan_area_width*pos.y];
}

UNROLL
void Evaluator::scanProximity(const ls::State& state, Evaluation& results) noexcept {
    struct PosStr {
        Position pos;
        unsigned dist_rem;
    };
    
    // Initialize env-buffer to contain only 0 and for fields occupied by snake 1
    // the LSB is set and for fields occupied by snake 2 the second bit is set.
    std::fill(envbuffer.begin(), envbuffer.end(), 0); //set all entries to zero
    const auto& snake1Pos = state.getSnake(0).getHeadPos();
    const auto& snake2Pos = state.getSnake(1).getHeadPos();
    const Position middle(scan_radius, scan_radius);
    for (const auto& tail : state.getSnake(0).getBody()) {
        if (isInEnvBuff(tail - snake1Pos + middle))
            getEnvBuffEntry(tail - snake1Pos + middle) |= 1;
        if (isInEnvBuff(tail - snake2Pos + middle))
            getEnvBuffEntry(tail - snake2Pos + middle) |= 2;
    }
    for (const auto& tail : state.getSnake(1).getBody()) {
        if (isInEnvBuff(tail - snake1Pos + middle))
            getEnvBuffEntry(tail - snake1Pos + middle) |= 1;
        if (isInEnvBuff(tail - snake2Pos + middle))
            getEnvBuffEntry(tail - snake2Pos + middle) |= 2;
    }
    for (const auto&  food : state.getFood()) {
        if (isInEnvBuff(food - snake1Pos + middle))
            getEnvBuffEntry(food - snake1Pos + middle) |= 4;
        if (isInEnvBuff(food - snake2Pos + middle))
            getEnvBuffEntry(food - snake2Pos + middle) |= 8;
    }
    // Additionally block the border of the bord of:
    for (uint32_t snake = 0; snake < 2; ++snake) {
        const uint8_t bit = 1<<snake;
        const auto& snakePos = state.getSnake(snake).getHeadPos();
        if (snakePos.x < scan_radius)
            for (unsigned y = 0; y < scan_area_width; y++)
                getEnvBuffEntry(Position(-1-snakePos.x + middle.x, y)) |= bit;
        if (snakePos.y < scan_radius)
            for (unsigned x = 0; x < scan_area_width; x++)
                getEnvBuffEntry(Position(x, -1-snakePos.y + middle.y)) |= bit;
        if (state.getWidth() - snakePos.x <= scan_radius)
            for (unsigned y = 0; y < scan_area_width; y++)
                getEnvBuffEntry(Position(state.getWidth()-snakePos.x + middle.x, y)) |= bit;
        if (state.getHeight() - snakePos.y <= scan_radius)
            for (unsigned x = 0; x < scan_area_width; x++)
                getEnvBuffEntry(Position(x, state.getHeight()-snakePos.y + middle.y)) |= bit;
    }
    //

    // Scan the environment
    for (uint32_t snake = 0; snake < 2; ++snake) {
        const uint8_t bit = 0b1<<snake;
        const uint8_t foodbit = 0b100<<snake;
        std::deque<PosStr> frontier = {PosStr{middle, scan_radius}};
        unsigned totalVisited = 0;
        unsigned foodReached = 0;
        while (!frontier.empty()) {
            auto cur = frontier.front();
            frontier.pop_front();
            if (cur.dist_rem > 0) {
                for (const auto dir : {Position(-1,0), Position(1,0), Position(0,-1), Position(0,1)}) {
                    const bool inbound = cur.pos.x > 0 && cur.pos.y && cur.pos.x < scan_area_width-1 && cur.pos.y < scan_area_width-1;
                    uint8_t& envbuff = getEnvBuffEntry(cur.pos + dir);
                    if (inbound && !(envbuff & bit)) {
                        envbuff |= bit;
                        frontier.push_back({cur.pos + dir, cur.dist_rem-1});
                        totalVisited++;
                        foodReached += !!(envbuff&foodbit);
                    }
                }
            }
        }
        results.snakes[snake].mobility = totalVisited;
        results.snakes[snake].mobility_per_area = totalVisited/(float)field_in_radius;
        results.snakes[snake].foodInReach = foodReached;
    }
}

static inline float relEval(float player, float opponent) noexcept {
	if (player >= opponent)
		return player/(player + opponent);
	return -opponent/(player + opponent);
}

float Evaluator::evaluate(const State& state) noexcept {
	auto eval = evaluate(state.state);
	if (eval.winner != ls::Winner::None) {
		if (eval.winner.containsAll(ls::Winner::Player1 | ls::Winner::Player2))
			return -50;
		else if (eval.winner.containsAny(ls::Winner::Player1))
			return 100;
		return -100;
	}
	return .2f*relEval(eval.snakes[0].health, eval.snakes[1].health) + 5*relEval(eval.snakes[0].mobility, eval.snakes[1].mobility) + .05f*relEval(eval.snakes[0].foodInReach, eval.snakes[1].foodInReach);
}