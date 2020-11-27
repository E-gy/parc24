#include <catch2ext.hpp>

int fun(int a, int b);

SCENARIO("stub", "[stub]"){
	REQUIRE(fun(2, 4) == 3);
}
