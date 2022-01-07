#include <catch2/catch.hpp>

#include <coroutine>
#include <utils/generator.h>

static Generator<unsigned> increment(unsigned until) {
	unsigned i = 0;
	for (; i < until; i++)
		co_yield i;
	co_return i;
}

TEST_CASE("Increment", "[Coroutine-Generator]") {
	auto routine = increment(5);
	REQUIRE((bool)routine);
	CHECK(routine() == 0);
	REQUIRE((bool)routine);
	CHECK(routine() == 1);
	REQUIRE((bool)routine);
	CHECK(routine() == 2);
	REQUIRE((bool)routine);
	CHECK(routine() == 3);
	REQUIRE((bool)routine);
	CHECK(routine() == 4);
	REQUIRE((bool)routine);
	CHECK(routine() == 5);
	CHECK_FALSE((bool)routine);
}