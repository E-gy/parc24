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
	string cmd1 = GENERATE("aaa", "c", "zzzzz", "echo");
	string cmd2 = GENERATE("bb", "jj", "k", "run");
	string cmd3 = GENERATE("bbcc", "yyy", "kj", "goodbye");
	GIVEN("an empty store"){
		CCMDStore store = ccmdstore_new();
		REQUIRE(!!store);
		REQUIRE(!ccmdstore_get(store, cmd1));
		REQUIRE(!ccmdstore_get(store, cmd2));
		REQUIRE(!ccmdstore_get(store, cmd3));
		THEN("operations are operational"){
			REQUIRE(IsOk(ccmdstore_set(store, cmd1, dummyf1)));
			REQUIRE(IsOk(ccmdstore_set(store, cmd2, dummyf2)));
			REQUIRE(IsOk(ccmdstore_set(store, cmd3, dummyf3)));
			REQUIRE(ccmdstore_get(store, cmd1) == dummyf1);
			REQUIRE(ccmdstore_get(store, cmd2) == dummyf2);
			REQUIRE(ccmdstore_get(store, cmd3) == dummyf3);
			AND_THEN("and replaceable"){
				REQUIRE(IsOk(ccmdstore_set(store, cmd1, dummyf4)));
				REQUIRE(ccmdstore_get(store, "echo") == dummyf4);
				REQUIRE(ccmdstore_get(store, cmd2) == dummyf2);
				REQUIRE(ccmdstore_get(store, cmd3) == dummyf3);
			}
		}
		ccmdstore_destroy(store);
	}
}
