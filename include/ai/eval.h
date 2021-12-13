#pragma once

#include <libsnake/state.h>
#include <libsnake/winner.h>

#include "state.h"

#include <inttypes.h>
#include <vector>

struct SnakeEval {
    unsigned mobility;
    float mobility_per_area;
    unsigned health;
    unsigned foodInReach;
	unsigned choice;
};

struct Evaluation {
    ls::Winner winner;
    SnakeEval snakes[2];
};

class Evaluator final {
private:
    const uint32_t scan_radius = 4;
    const uint32_t scan_area_width = 2*scan_radius+1;
    const uint32_t field_in_radius = (scan_radius * (scan_radius+1) * 2);

    std::vector<uint8_t> envbuffer;

    inline void scanProximity(const ls::State& state, Evaluation& results) noexcept;
    inline bool isInEnvBuff(const ls::Position& pos) const noexcept;
    inline uint8_t& getEnvBuffEntry(const ls::Position& pos) noexcept;

    Evaluator(const Evaluator& other) = delete;
    Evaluator& operator=(const Evaluator& other) = delete;
public:
    Evaluator() noexcept;

    Evaluation evaluate(const ls::State& state) noexcept;

	float evaluate(const State& state) noexcept;
};