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
			IfElse_T(realias(args0, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args1, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args2, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args3, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args4, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
		}
	}
	GIVEN("store with another alias"){
		CHECK(IsOk(aliastore_add(aliases, "hi", "wroom")));
		THEN("no replacements can be made"){
			IfElse_T(realias(args0, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args1, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args2, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args3, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args4, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
		}
	}
	GIVEN("store with relevant alias"){
		CHECK(IsOk(aliastore_add(aliases, "hello", "wroom")));
		THEN("replacements made where due"){
			IfElse_T(realias(args0, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			string expected1[] = {"wroom", null};
			IfElse_T(realias(args1, aliases), args, {
				REQUIRE(args != nulla);
				REQUIRE(args->size == 1);
				for(size_t i = 0; i < args->size; i++) REQUIRE_THAT(args->args[i], Equals(expected1[i]));
				argsarrmut_destroy(args);
			}, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			string expected2[] = {"wroom", "world", null};
			IfElse_T(realias(args2, aliases), args, {
				REQUIRE(args != nulla);
				REQUIRE(args->size == 2);
				for(size_t i = 0; i < args->size; i++) REQUIRE_THAT(args->args[i], Equals(expected2[i]));
				argsarrmut_destroy(args);
			}, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args3, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args4, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
		}
	}
	GIVEN("store with relevant multiarg alias"){
		CHECK(IsOk(aliastore_add(aliases, "hello", "big chungus vs ultra instinct shaggy")));
		THEN("replacements made where due"){
			IfElse_T(realias(args0, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			string expected1[] = {"big", "chungus", "vs", "ultra", "instinct", "shaggy", null};
			IfElse_T(realias(args1, aliases), args, {
				REQUIRE(args != nulla);
				REQUIRE(args->size == 6);
				for(size_t i = 0; i < args->size; i++) REQUIRE_THAT(args->args[i], Equals(expected1[i]));
				argsarrmut_destroy(args);
			}, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			string expected2[] = {"big", "chungus", "vs", "ultra", "instinct", "shaggy", "world", null};
			IfElse_T(realias(args2, aliases), args, {
				REQUIRE(args != nulla);
				REQUIRE(args->size == 7);
				for(size_t i = 0; i < args->size; i++) REQUIRE_THAT(args->args[i], Equals(expected2[i]));
				argsarrmut_destroy(args);
			}, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args3, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args4, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
		}
	}
	GIVEN("store with relevant aliases"){
		CHECK(IsOk(aliastore_add(aliases, "hello", "wroom ")));
		CHECK(IsOk(aliastore_add(aliases, "world", "boom boom")));
		THEN("replacements made where due"){
			IfElse_T(realias(args0, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			string expected1[] = {"wroom", null};
			IfElse_T(realias(args1, aliases), args, {
				REQUIRE(args != nulla);
				REQUIRE(args->size == 1);
				for(size_t i = 0; i < args->size; i++) REQUIRE_THAT(args->args[i], Equals(expected1[i]));
				argsarrmut_destroy(args);
			}, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			string expected2[] = {"wroom", "boom", "boom", null};
			IfElse_T(realias(args2, aliases), args, {
				CAPTURE(args->args[0]);
				CAPTURE(args->args[1]);
				REQUIRE(args != nulla);
				REQUIRE(args->size == 3);
				for(size_t i = 0; i < args->size; i++) REQUIRE_THAT(args->args[i], Equals(expected2[i]));
				argsarrmut_destroy(args);
			}, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args3, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
			IfElse_T(realias(args4, aliases), a, { REQUIRE(a == nulla); }, err, { FAIL_FMT("Realias failed with - %s", err.s); });
		}
	}
	aliastore_destroy(aliases);
}
