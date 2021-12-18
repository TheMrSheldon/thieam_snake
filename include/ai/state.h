#pragma once

#include <libsnake/move.h>
#include <libsnake/state.h>
#include <libsnake/gamemode.h>
#include <libsnake/gamemodes/arena.h>

#include <iostream>
#include <bitset>
#include <vector>

class State {
	using LState = ls::State;
public:
	const ls::Gamemode& gamemode = ls::gm::Arena;
	const std::vector<ls::Move> moves;
	LState state;

public:
	State(LState state) : state(state), moves() {}
	State(LState state, std::vector<ls::Move>&& moves) : state(state), moves(moves) {}

	const ls::Gamemode& getGamemode() const noexcept {
		return gamemode;
	}
	unsigned getWidth() const noexcept {
		return state.getWidth();
	}
	unsigned getHeight() const noexcept {
		return state.getHeight();
	}

	size_t getTurn() const noexcept {
		return moves.size();
	}

	ls::Move getValidActions() const noexcept {
		ls::Move actions = gamemode.getUnblockedActions(state, getTurn());
		if (actions == ls::Move::none)
			return ls::Move::up;
		return actions;
	}

	bool isGameOver() const noexcept {
		return gamemode.isGameOver(state);
	}

	State afterMove(ls::Move action) const noexcept {
		std::vector<ls::Move> newMoves(moves.begin(), moves.end());
		newMoves.emplace_back(action);
		if (newMoves.size() == state.getSnakes().size())
			return State(gamemode.stepState(state, newMoves));
		return State(state, std::move(newMoves));
	}
};