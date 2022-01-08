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

TEST_CASE("Multiple", "[Coroutine-Generator]") {
	std::vector<Generator<unsigned>> routines;
	for (int i = 0; i < 2; i++)
		routines.emplace_back(increment(1+2*i));
	REQUIRE((bool)(routines[0]));
	CHECK((routines[0]()) == 0);
	REQUIRE((bool)(routines[1]));
	CHECK(routines[1]() == 0);

	REQUIRE((bool)routines[0]);
	CHECK((routines[0]()) == 1);
	REQUIRE((bool)(routines[1]));
	CHECK((routines[1]()) == 1);
	
	CHECK_FALSE((bool)(routines[0]));
	REQUIRE((bool)(routines[1]));
	CHECK((routines[1]()) == 2);
	
	REQUIRE((bool)(routines[1]));
	CHECK((routines[1]()) == 3);
	
	CHECK_FALSE((bool)(routines[1]));
}