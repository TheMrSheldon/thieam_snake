#pragma once

#include <libsnake/state.h>
#include <libsnake/snake_flags.h>

#include "state.h"

#include <inttypes.h>
#include <vector>

struct SnakeEval final {
	unsigned mobility;
	float mobility_per_area;
	unsigned health;
	unsigned foodInReach;
	unsigned choice;
};

struct Evaluation final {
	ls::SnakeFlags winner;
	std::vector<SnakeEval> snakes;
};

class EnvBuffer final {
private:
	struct Entry {
		int8_t timeBlocked;
		ls::SnakeFlags snake;
	};
	const unsigned width;
	const unsigned height;
	std::vector<Entry> data;
	std::vector<size_t> areaControl;
	std::vector<size_t> borderControl;

	EnvBuffer(const EnvBuffer& other) = delete;
	EnvBuffer& operator=(const EnvBuffer& other) = delete;

	const Entry& getEntry(const ls::Position& pos) const noexcept {
		ASSERT(isInbounds(pos), "Position is out of bounds.");
		return data[pos.y*width+pos.x];
	}
	Entry& getEntry(const ls::Position& pos) noexcept {
		ASSERT(isInbounds(pos), "Position is out of bounds.");
		return data[pos.y*width+pos.x];
	}
public:
	inline EnvBuffer(size_t numSnakes, unsigned width, unsigned height) noexcept : width(width), height(height),
		data(width*height, Entry{}), areaControl(numSnakes, 0), borderControl(numSnakes, 0) {}
	inline void storeSnake(size_t snakeIdx, const ls::Snake& snake) noexcept {
		for (size_t bodyIdx = 0; bodyIdx < snake.length(); ++bodyIdx)
			blockUntilTurn(snake.getBody()[bodyIdx], snakeIdx, snake.length()-1-bodyIdx);
	}
	inline void clear() noexcept {
		std::memset(data.data(), 0, data.size()*sizeof(decltype(data)::value_type));
	}
	inline bool isInbounds(const ls::Position& pos) const noexcept {
		return pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height;
	}
	inline bool isBlockedAtTurn(const ls::Position& pos, size_t snake, size_t turn) const noexcept {
		ASSERT(turn <= 0b01111111, "TODO: Why?");
		if (!isInbounds(pos))
			return true;
		const auto& entry = getEntry(pos);
		if (entry.snake == 0)
			return false;
		if (entry.timeBlocked <= 0) {//Field is "Blocked Until"
			return turn <= (-entry.timeBlocked);
		} else {//Field is "Blocked after"
			if ((entry.snake & ls::SnakeFlags::ByIndex(snake)) != ls::SnakeFlags::None) {//This snake already claimed the field
				ASSERT(turn >= entry.timeBlocked, "TODO: Why?");
				return true;
			}
			//If another snake wants to check if the field is blocked at this time but would reach the field
			//at the same time as the snake who previously claimed it (turn == entry.timeBlocked) then this
			//is a border-tile and thus is counted as "unblocked" for both snakes.
			return turn > entry.timeBlocked;
		}
	}
	inline void blockAfterTurn(const ls::Position& pos, size_t snake, size_t turn) noexcept {
		ASSERT(turn <= 0b01111111, "TODO: Why?");
		getEntry(pos) = {
			.timeBlocked = (int8_t)turn,
			.snake = ls::SnakeFlags::ByIndex(snake)
		};
		//FIXME: count as border if two or more snakes access at the same time
		ASSERT(snake < areaControl.size(), "Snake index out of range");
		areaControl[snake]++;
	}
	inline void blockUntilTurn(const ls::Position& pos, size_t snake, size_t turn) noexcept {
		ASSERT(turn <= 0b01111111, "TODO: Why?");
		getEntry(pos) = {
			.timeBlocked = -(int8_t)turn,
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
	EnvBuffer envbuffer;

	inline void scanProximity(const ls::State& state, Evaluation& results) noexcept;

	Evaluator(const Evaluator& other) = delete;
	Evaluator& operator=(const Evaluator& other) = delete;
public:
	Evaluator(const ls::Gamemode& gamemode, unsigned numSnakes, unsigned width, unsigned height) noexcept;

	Evaluation evaluate(const ls::State& state) noexcept;

	float evaluate(const State& state) noexcept;
};