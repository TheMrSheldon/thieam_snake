#pragma once

#include <libsnake/move.h>
#include <libsnake/state.h>
#include <libsnake/gamemode.h>
#include <libsnake/gamemodes/duel.h>

#include <iostream>
#include <bitset>

class State {
	using LState = ls::State;
public:
	const ls::Gamemode<2>& gamemode = ls::gm::Duel;
	Move p1Move;
	LState state;

public:
	State(LState state, Move p1Move = 0) : state(state), p1Move(p1Move) {}

	int getTurn() const noexcept {
		return (p1Move == 0)? 0: 1;
	}

	std::vector<Move> getValidActions() const noexcept {
		auto move = state.getPossibleActions(getTurn());
		std::vector<Move> ret;
		if (isUp(move))
			ret.emplace_back(MoveUp);
		if (isDown(move))
			ret.emplace_back(MoveDown);
		if (isLeft(move))
			ret.emplace_back(MoveLeft);
		if (isRight(move))
			ret.emplace_back(MoveRight);
		return ret;
	}

	bool isGameOver() const noexcept {
		return gamemode.isGameOver(state);
	}

	State afterMove(Move action) const noexcept {
		if (p1Move != 0)
			return State(gamemode.stepState(state, {p1Move, action}));
		return State(state, action);
	}
};