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
	ls::Move p1Move;
	LState state;

public:
	State(LState state, ls::Move p1Move = 0) : state(state), p1Move(p1Move) {}

	int getTurn() const noexcept {
		return (p1Move == 0)? 0: 1;
	}

	ls::Move getValidActions() const noexcept {
		ls::Move actions = ls::gm::Duel.getUnblockedActions(state, getTurn());
		if (actions == ls::Move::none)
			return ls::Move::up;
		return actions;
	}

	bool isGameOver() const noexcept {
		return gamemode.isGameOver(state);
	}

	State afterMove(ls::Move action) const noexcept {
		if (p1Move != 0)
			return State(gamemode.stepState(state, {p1Move, action}));
		return State(state, action);
	}
};