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
	GIVEN("automaton accepting only letters 'd','e', or 'f'"){
		State a = patstate_new(false);
		State acc = patstate_new(true);
		patstate_tradd(a, patransition_new('e', acc));
		patstate_tradd(a, patransition_new('f', acc));
		patstate_tradd(a, patransition_new('d', acc));
		THEN("said single char strings are accepted"){
			auto str = GENERATE("d", "e", "f");
			REQUIRE(auto_test(a, str));
		}
		THEN("everything else is rejected"){
			auto str = GENERATE("j", "", "efvsgvd", "-", "*", "-+533ge w4r 544", " ");
			REQUIRE(!auto_test(a, str));
		}
		auto_destroy(a);
	}
}

SCENARIO("automata operations", "[patterns]"){
	GIVEN("an automaton accepting 1 letter strings or 'abc' or 'hi'"){
		State a = patstate_new(false);
		State aa = patstate_new(true);
		State aab = patstate_new(false);
		patstate_tradd(aab, patransition_new('c', patstate_new(true)));
		patstate_tradd(aa, patransition_new('b', aab));
		patstate_tradd(a, patransition_new('a', aa));
		State ah = patstate_new(false);
		patstate_tradd(ah, patransition_new('i', patstate_new(true)));
		patstate_tradd(a, patransition_new('h', ah));
		a->defolt = patstate_new(true);
		THEN("automaton accepts as expected"){
			REQUIRE(auto_test(a, "a"));
			REQUIRE(auto_test(a, "d"));
			REQUIRE(auto_test(a, "+"));
			REQUIRE(auto_test(a, " "));
			REQUIRE(auto_test(a, "abc"));
			REQUIRE(auto_test(a, "hi"));
			REQUIRE(!auto_test(a, ""));
			REQUIRE(!auto_test(a, "grr"));
			REQUIRE(!auto_test(a, "  "));
		}
		WHEN("negating the automaton"){
			auto_negate(a);
			THEN("automaton behaviour is flipped"){
				REQUIRE(!auto_test(a, "a"));
				REQUIRE(!auto_test(a, "d"));
				REQUIRE(!auto_test(a, "+"));
				REQUIRE(!auto_test(a, " "));
				REQUIRE(!auto_test(a, "abc"));
				REQUIRE(!auto_test(a, "hi"));
				REQUIRE(auto_test(a, ""));
				REQUIRE(auto_test(a, "grr"));
				REQUIRE(auto_test(a, "  "));
			}
		}
		auto_destroy(a);
	}
	GIVEN("an automaton accepting 3 letter words, and an automaton accepting words made only of 'a's"){
		State a3 = patstate_new(false);
		((a3->defolt = patstate_new(false))->defolt = patstate_new(false))->defolt = patstate_new(true);
		REQUIRE(auto_test(a3, "asf"));
		REQUIRE(auto_test(a3, "aaa"));
		REQUIRE(auto_test(a3, "   "));
		REQUIRE(!auto_test(a3, "    "));
		REQUIRE(!auto_test(a3, "  "));
		REQUIRE(!auto_test(a3, ""));
		State aa = patstate_new(false);
		State aaa = patstate_new(true);
		patstate_tradd(aaa, patransition_new('a', aaa));
		patstate_tradd(aa, patransition_new('a', aaa));
		REQUIRE(auto_test(aa, "a"));
		REQUIRE(auto_test(aa, "aaaaaaaaaa"));
		REQUIRE(auto_test(aa, "aa"));
		REQUIRE(auto_test(aa, "aaa"));
		REQUIRE(!auto_test(aa, ""));
		REQUIRE(!auto_test(aa, "abc"));
		REQUIRE(!auto_test(aa, "aaaaaae"));
		REQUIRE(!auto_test(aa, "eeeeeea"));
		THEN("the intersection only accepts 'aaa'"){
			State aAa = auto_merge(a3, aa, false);
			REQUIRE(auto_test(aAa, "aaa"));
			auto rejected = GENERATE("", "asf", "   ", "    ", "  ", "", "a", "aaaaaaaaaa", "aa", "", "abc", "aaaaaae", "eeeeeea");
			REQUIRE(!auto_test(aAa, rejected));
			auto_destroy(aAa);
		}
		THEN("the union accepts whatever either would"){
			State aAa = auto_merge(a3, aa, true);
			THEN("accepting"){
				auto accepted = GENERATE("asf", "aaa", "   ", "a", "aaaaaaaaaa", "aa");
				REQUIRE(auto_test(aAa, accepted));
			}
			THEN("rejecting"){
				auto rejected = GENERATE("    ", "  ", "", "aaaaaae", "eeeeeea");
				REQUIRE(!auto_test(aAa, rejected));
			}
			auto_destroy(aAa);
		}
		auto_destroy(aa);
		auto_destroy(a3);
	}
}
