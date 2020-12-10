#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <pat/pati.h>
#include <util/null.h>
}

SCENARIO("baseline automata", "[patterns]"){
	GIVEN("no automaton"){
		auto str = GENERATE("sfv", "a", " ", "ddfdv **--+ 43t53", "");
		THEN("nothing is accepted"){
			REQUIRE(!auto_test(null, str));
		}
	}
	GIVEN("1 state rejecting automaton"){
		State a = patstate_new(false);
		auto str = GENERATE("sfv", "a", " ", "ddfdv **--+ 43t53", "");
		THEN("nothing is accepted"){
			REQUIRE(!auto_test(a, str));
		}
		auto_destroy(a);
	}
	GIVEN("1 state accepting automaton"){
		State a = patstate_new(true);
		THEN("empty string is accpeted"){
			REQUIRE(auto_test(a, ""));
			auto str = GENERATE("sfv", "a", " ", "ddfdv **--+ 43t53");
			AND_THEN("and nothing else"){
				REQUIRE(!auto_test(a, str));
			}
		}
		auto_destroy(a);
	}
	GIVEN("2 state 1 character transition automaton"){
		State a = patstate_new(false);
		State b = patstate_new(true);
		patstate_tradd(a, patransition_new('a', b));
		THEN("said char string is accepted"){
			REQUIRE(auto_test(a, "a"));
			auto str = GENERATE("sfv", "", " ", "ddfdv **--+ 43t53");
			AND_THEN("and nothing else"){
				REQUIRE(!auto_test(a, str));
			}
		}
		auto_destroy(a);
	}
	GIVEN("2 state 1 defolt transition automaton"){
		State a = patstate_new(false);
		State b = patstate_new(true);
		a->defolt = b;
		THEN("single char strings are accepted"){
			auto str = GENERATE("a", "v", "d", " ", "	", "+");
			REQUIRE(auto_test(a, str));
		}
		THEN("not single char strings are rejected"){
			auto str = GENERATE("sfv", "", "ddfdv **--+ 43t53");
			REQUIRE(!auto_test(a, str));
		}
		auto_destroy(a);
	}
}
