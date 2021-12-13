#pragma once

#include "gameinfo.h"

#include <libsnake/state.h>
#include <libsnake/move.h>

#include <string>

class Agent {
public:
	const std::string author;
	const std::string color;
	const std::string head;
	const std::string tail;
	const std::string version;
private:
	Agent(const Agent& other) = delete;
	Agent& operator=(const Agent& other) = delete;
protected:
	Agent(std::string author, std::string color, std::string head, std::string tail, std::string version) noexcept
	 : author(author), color(color), head(head), tail(tail), version(version) {}
public:
	virtual ~Agent() = default;

	virtual void startGame(const GameInfo& info) = 0;
	virtual ls::Move getAction(const GameInfo& info, uint32_t turn, ls::State& state) = 0;
	virtual void endGame(const GameInfo& info) = 0;
};