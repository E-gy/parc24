#include <catch2ext.hpp>

extern "C" {
int fun(int a, int b);
}

SCENARIO("stub", "[stub]"){
	REQUIRE(fun(2, 4) == 3);
}
