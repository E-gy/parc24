#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <parc24/ccmdstore.h>
#include <util/null.h>

static int dummyf1(int argc, argsarr args){ return 0; }
static int dummyf2(int argc, argsarr args){ return 0; }
static int dummyf3(int argc, argsarr args){ return 0; }
static int dummyf4(int argc, argsarr args){ return 0; }

}

SCENARIO("ccmds store is operational", "[ccmds store][parc24]"){
	GIVEN("an empty store"){
		CCMDStore store = ccmdstore_new();
		REQUIRE(!!store);
		REQUIRE(!ccmdstore_get(store, "echo"));
		REQUIRE(!ccmdstore_get(store, "run"));
		REQUIRE(!ccmdstore_get(store, "goodbye"));
		THEN("operations are operational"){
			REQUIRE(IsOk(ccmdstore_set(store, "echo", dummyf1)));
			REQUIRE(IsOk(ccmdstore_set(store, "run", dummyf2)));
			REQUIRE(IsOk(ccmdstore_set(store, "goodbye", dummyf3)));
			REQUIRE(ccmdstore_get(store, "echo") == dummyf1);
			REQUIRE(ccmdstore_get(store, "run") == dummyf2);
			REQUIRE(ccmdstore_get(store, "goodbye") == dummyf3);
			AND_THEN("and replaceable"){
				REQUIRE(IsOk(ccmdstore_set(store, "echo", dummyf4)));
				REQUIRE(ccmdstore_get(store, "echo") == dummyf4);
				REQUIRE(ccmdstore_get(store, "run") == dummyf2);
				REQUIRE(ccmdstore_get(store, "goodbye") == dummyf3);
			}
		}
		ccmdstore_destroy(store);
	}
}
