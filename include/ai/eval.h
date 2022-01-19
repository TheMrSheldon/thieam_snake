#pragma once

#include <libsnake/state.h>
#include <libsnake/snake_flags.h>

#include "state.h"
#include "state_of_mind.h"
#include "search.h"

#include <inttypes.h>
#include <map>
#include <vector>

struct SnakeEval final {
	unsigned mobility;
	unsigned border;
	float mobility_per_area;
	unsigned foodInReach;
	unsigned choice;
};

struct Evaluation final {
	ls::SnakeFlags winner;
	std::vector<SnakeEval> snakes;
	unsigned depth;
};

class EnvBuffer final {
private:
	struct Entry {
		struct {
			bool until : 1;
			uint8_t turn : 7;
		} timeBlocked;
		ls::SnakeFlags snake;
	};
	const unsigned width;
	const unsigned height;
	std::vector<Entry> data;
	std::vector<size_t> areaControl;
	std::vector<size_t> borderControl;

	EnvBuffer(const EnvBuffer& other) = delete;
	EnvBuffer& operator=(const EnvBuffer& other) = delete;

	inline Entry& getEntry(const ls::Position& pos) noexcept {
		ASSERT(isInbounds(pos), "Position is out of bounds.");
		return data[pos.y*width+pos.x];
	}
public:
	inline EnvBuffer(size_t numSnakes, unsigned width, unsigned height) noexcept : width(width), height(height),
		data(width*height, Entry{}), areaControl(numSnakes, 0), borderControl(numSnakes, 0) {}
	inline EnvBuffer(EnvBuffer&& other) noexcept : width(other.width), height(other.height), data(std::move(other.data)),
		areaControl(std::move(other.areaControl)), borderControl(std::move(other.borderControl)) {}
	inline void storeSnake(size_t snakeIdx, const ls::Snake& snake) noexcept {
		for (size_t bodyIdx = 0; bodyIdx < snake.length(); ++bodyIdx)
			blockUntilTurn(snake.getBody()[bodyIdx], snakeIdx, snake.length()-1-bodyIdx);
	}
	inline void clear() noexcept {
		std::memset(data.data(), 0, data.size()*sizeof(decltype(data)::value_type));
	}
	inline bool isInbounds(const ls::Position& pos) const noexcept {
		return pos.x >= 0 && pos.y >= 0 && (unsigned)pos.x < width && (unsigned)pos.y < height;
	}
	inline unsigned getWidth() const noexcept { return width; }
	inline unsigned getHeight() const noexcept { return height; }
	inline const Entry& getEntry(const ls::Position& pos) const noexcept {
		ASSERT(isInbounds(pos), "Position is out of bounds.");
		return data[pos.y*width+pos.x];
	}
	inline bool isBlockedAtTurn(const ls::State& state, const ls::Position& pos, size_t snake, size_t turn) const noexcept {
		ASSERT(turn <= 0b01111111, "TODO: Why?");
		if (!isInbounds(pos))
			return true;
		const auto& entry = getEntry(pos);
		if (entry.snake == ls::SnakeFlags::None)
			return false;
		if (entry.timeBlocked.until) {//Field is "Blocked Until"
			return turn <= entry.timeBlocked.turn;
		} else {//Field is "Blocked after"
			if ((entry.snake & ls::SnakeFlags::ByIndex(snake)) != ls::SnakeFlags::None) {//This snake already claimed the field
				ASSERT(turn >= entry.timeBlocked.turn, "TODO: Why?");
				return true;
			}
			//If another snake wants to check if the field is blocked at this time but would reach the field
			//at the same time as the snake who previously claimed it (turn == entry.timeBlocked) then this
			//is a border-tile and thus is counted as "unblocked" for the longer snake.
			return turn > entry.timeBlocked.turn || state.getSnake(entry.snake.getIndex()).length() > state.getSnake(snake).length();
			//FIXME: entry.snake.getIndex() may throw an error if more than two snakes could occupy the field
		}
	}
	inline void blockAfterTurn(const ls::Position& pos, size_t snake, size_t turn) noexcept {
		ASSERT(turn <= 0b01111111, "TODO: Why?");
		ASSERT(snake < areaControl.size(), "Snake index out of range");
		ASSERT(snake < borderControl.size(), "Snake index out of range");
		auto& entry = getEntry(pos);
		if (entry.timeBlocked.turn != turn) {
			entry.timeBlocked = {.until=false, .turn=(uint8_t)turn};
			entry.snake = ls::SnakeFlags::ByIndex(snake);
			areaControl[snake]++;
		} else { // A snake already blocked this field
			if (entry.snake.size() == 1) {//only one snake previously visited
				const auto snakeIdx = entry.snake.getIndex();
				areaControl[snakeIdx]--;
				borderControl[snakeIdx]++;
			}
			entry.snake |= ls::SnakeFlags::ByIndex(snake);
			borderControl[snake]++;
		}
	}
	inline void blockUntilTurn(const ls::Position& pos, size_t snake, size_t turn) noexcept {
		ASSERT(turn <= 0b01111111, "TODO: Why?");
		getEntry(pos) = {
			.timeBlocked = {.until=true, .turn=(uint8_t)turn},
			.snake = ls::SnakeFlags::ByIndex(snake)
		};
	}

	inline size_t getAreaControl(size_t snake) const noexcept {
		ASSERT(snake < areaControl.size(), "Snake index out of range");
		return areaControl[snake];
	}
	inline size_t getBorderControl(size_t snake) const noexcept {
		ASSERT(snake < borderControl.size(), "Snake index out of range");
		return borderControl[snake];
	}
};

class Evaluator final {
private:
	const ls::Gamemode& gamemode;
	const std::map<ls::SnakeFlags, StateOfMind> mind;
	EnvBuffer envbuffer;

	inline void scanProximity(const ls::State& state, unsigned depth, Evaluation& results) noexcept;

	Evaluator(const Evaluator& other) = delete;
	Evaluator& operator=(const Evaluator& other) = delete;
public:
	Evaluator(const ls::Gamemode& gamemode, unsigned numSnakes, unsigned width, unsigned height, const std::map<ls::SnakeFlags, StateOfMind>& mind) noexcept;
	Evaluator(Evaluator&& other) noexcept;

	const EnvBuffer& getEnvBuffer() const noexcept { return envbuffer; }
	
	Evaluation evaluate(const ls::State& state, unsigned depth) noexcept;
	float evaluate(const State& state, unsigned depth) noexcept;
	std::map<ls::SnakeFlags, float> evaluateAll(const State& state, unsigned depth) noexcept;
};