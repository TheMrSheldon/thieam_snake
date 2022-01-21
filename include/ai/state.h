#pragma once

#include <libsnake/move.h>
#include <libsnake/state.h>
#include <libsnake/gamemode.h>
#include <libsnake/gamemodes/standard.h>

#include <iostream>
#include <set>
#include <vector>

class State {
	using LState = ls::State;
	using LGamemode = ls::Gamemode;
public:
	const LGamemode& gamemode;
	const std::vector<ls::Move> moves;
	LState state;

public:
	State(const LGamemode& gamemode, LState state) : gamemode(gamemode), state(state), moves() {}
	State(const LGamemode& gamemode, LState state, std::vector<ls::Move>&& moves) : gamemode(gamemode), state(state), moves(moves) {}

	const ls::Gamemode& getGamemode() const noexcept {
		return gamemode;
	}
	unsigned getWidth() const noexcept {
		return state.getWidth();
	}
	unsigned getHeight() const noexcept {
		return state.getHeight();
	}
	size_t getNumParties() const noexcept {
		std::set<ls::SnakeFlags> squads;
		for (const auto& snake : state.getSnakes())
			if (!snake.isDead())
				squads.insert(snake.getSquad());
		return squads.size();
	}
	size_t getNumPlayers() const noexcept {
		return state.getNumSnakes();
	}

	ls::SnakeFlags getCurrentParty() const noexcept {
		return state.getSnake(getTurn()).getSquad();
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
		//Dead snakes make no moves
		while (newMoves.size() < state.getSnakes().size() && state.getSnake(newMoves.size()).isDead())
			newMoves.emplace_back(ls::Move::none);
		if (newMoves.size() == state.getNumSnakes())
			return State(gamemode, gamemode.stepState(state, newMoves));
		return State(gamemode, state, std::move(newMoves));
	}
};