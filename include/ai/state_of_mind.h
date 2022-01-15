#pragma once

#include <libsnake/state.h>

class StateOfMind final {
private:

public:
    float getRating(ls::State& state, struct Evaluation& eval) const noexcept;
};