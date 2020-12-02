#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <grammar/aliaser.h>
#include <util/null.h>
}

#define nulla ((ArgsArr_Mut) null)

SCENARIO("aliaser does the aliasing", "[alias][exe]"){
	string args0[] = {null};
	string args1[] = {"hello", null};
	string args2[] = {"hello", "world", null};
	string args3[] = {"\\hello", "world", null};
	string args4[] = {"hello brave new", "world", null};
	AliasStore aliases = aliastore_new();
	GIVEN("empty store"){
		THEN("no replacements can be made"){
			IfElse_T(realias(args0, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args1, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args2, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args3, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args4, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
		}
	}
	GIVEN("store with another alias"){
		CHECK(IsOk(aliastore_add(aliases, "hi", "wroom")));
		THEN("no replacements can be made"){
			IfElse_T(realias(args0, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args1, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args2, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args3, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
			IfElse_T(realias(args4, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realiase failed with - %s", err.s); });
		}
	}
	aliastore_destroy(aliases);
}
