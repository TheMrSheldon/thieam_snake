#pragma once

#include <libsnake/move.h>
#include <libsnake/state.h>
#include <libsnake/gamemode.h>
#include <libsnake/gamemodes/standard.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <numeric>

class State {
	using LState = ls::State;
	using LGamemode = ls::Gamemode;
public:
	const LGamemode& gamemode;
	const std::vector<ls::Move> moves;
	LState state;
	uint32_t fullTurn;

public:
	State(const LGamemode& gamemode, uint32_t fullTurn, LState state) : gamemode(gamemode), fullTurn(fullTurn), state(state), moves() {}
	State(const LGamemode& gamemode, uint32_t fullTurn, LState state, std::vector<ls::Move>&& moves) : gamemode(gamemode), fullTurn(fullTurn), state(state), moves(moves) {}

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
		return state.getLivingSquads().size();
	}
	size_t getNumPlayers() const noexcept {
		//return state.getNumSnakes();
		const auto& squads = state.getLivingSquads();
		return std::accumulate(squads.begin(), squads.end(), (size_t)0, [](size_t left, const ls::SnakeFlags& right){ return left+right.size(); });
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
			return State(gamemode, fullTurn+1, gamemode.stepState(state, fullTurn, newMoves));
		return State(gamemode, fullTurn, state, std::move(newMoves));
	}
};