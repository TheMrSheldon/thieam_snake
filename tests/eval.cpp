#include <catch2/catch.hpp>

#include <ai/eval.h>

#include <vector>

using namespace ls;

#if 0
TEST_CASE("State1", "[Evaluation]") {
    /**
     * ╔═══════════════════╗    Snake 1:    <11
     * ║ . . . . o . . 2 . ║    Snake 2:    <22
     * ║ . . . . . o . 2 . ║    Food:       o
     * ║ . 1 1 1 1 1 . 2 . ║    Empty:      .
     * ║ . . . . . 1 . ^ . ║
     * ║ . o . < 1 1 . . . ║
     * ║ . . . . . . . . o ║
     * ╚═══════════════════╝
     */
    std::vector<Position> snake1 = {{3,1},{4,1},{5,1},{5,2},{5,3},{4,3},{3,3},{2,3},{1,3}};
    std::vector<Position> snake2 = {{7,2},{7,3},{7,4},{7,5}};
    std::vector<Position> food = {{8,0},{1,1},{5,4},{4,5}};
    auto sdata1 = SnakeData({snake1.data(), (uint32_t)snake1.size()}, MoveLeft, 82);
    auto sdata2 = SnakeData({snake2.data(), (uint32_t)snake2.size()}, MoveDown, 19);
    auto state = State(9,6, {sdata1, sdata2}, {food.data(), (uint32_t)food.size()});

    //Assert correct evaluation
    Evaluator evaluator;
    auto eval = evaluator.evaluate(state);
    CHECK(eval.winner == Winner::None);
    CHECK(eval.snakes[0].health == sdata1.health);
    CHECK(eval.snakes[1].health == sdata2.health);
    // Mobility:
    // ╔═══════════════════╗ ╔═══════════════════╗ +: Mobility of 1
    // ║ + + + + + + + . o ║ ║ . . . . . # # # # ║ #: Mobility of 2
    // ║ + + + < 1 1 . . . ║ ║ . o . < 1 1 # # # ║
    // ║ + + + + + 1 . ^ . ║ ║ . . . . . 1 # ^ # ║
    // ║ . 1 1 1 1 1 . 2 . ║ ║ . 1 1 1 1 1 # 2 # ║
    // ║ . . . . . o . 2 . ║ ║ . . . . . # # 2 # ║
    // ║ . . . . o . . 2 . ║ ║ . . . . o . # 2 # ║
    // ╚═══════════════════╝ ╚═══════════════════╝
    CHECK(eval.snakes[0].mobility == 15);
    CHECK(eval.snakes[1].mobility == 16);
    CHECK(eval.snakes[0].foodInReach == 1);
    CHECK(eval.snakes[1].foodInReach == 2);
}
#endif