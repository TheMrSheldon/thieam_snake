#pragma once

#include <libsnake/snake_flags.h>
#include <libsnake/state.h>

class StateOfMind final {
public:
	enum class Target {
		Grow, Control
	};
private:
	ls::SnakeFlags squad;
	Target target;

public:
	StateOfMind(ls::SnakeFlags squad) noexcept;

	void setTarget(Target target) noexcept;

	float getRating(const ls::State& state, struct Evaluation& eval) const noexcept;
};