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
		WHEN("applying ? on the automaton"){
			Automaton aopt = auto_optional(a);
			THEN("automaton accepts same things and empty string"){
				REQUIRE(auto_test(aopt, "a"));
				REQUIRE(auto_test(aopt, "d"));
				REQUIRE(auto_test(aopt, "+"));
				REQUIRE(auto_test(aopt, " "));
				REQUIRE(auto_test(aopt, "abc"));
				REQUIRE(auto_test(aopt, "hi"));
				REQUIRE(auto_test(aopt, ""));
				REQUIRE(!auto_test(aopt, "grr"));
				REQUIRE(!auto_test(aopt, "  "));
			}
			auto_destroy(aopt);
		}
		auto_destroy(a);
	}
	GIVEN("an automaton accepting 'a' and one accepting 'b'"){
		State a = patstate_new(false);
		patstate_tradd(a, patransition_new('a', patstate_new(true)));
		REQUIRE(auto_test(a, "a"));
		REQUIRE(!auto_test(a, "b"));
		REQUIRE(!auto_test(a, ""));
		REQUIRE(!auto_test(a, "aaa"));
		REQUIRE(!auto_test(a, "bbb"));
		REQUIRE(!auto_test(a, "ab"));
		REQUIRE(!auto_test(a, "ba"));
		State b = patstate_new(false);
		patstate_tradd(b, patransition_new('b', patstate_new(true)));
		REQUIRE(auto_test(b, "b"));
		REQUIRE(!auto_test(b, "a"));
		REQUIRE(!auto_test(b, ""));
		REQUIRE(!auto_test(b, "aaa"));
		REQUIRE(!auto_test(b, "bbb"));
		REQUIRE(!auto_test(a, "ab"));
		REQUIRE(!auto_test(a, "ba"));
		THEN("the concatenation only accepts 'ab'"){
			State ab = auto_concat(a, b);
			REQUIRE(auto_test(ab, "ab"));
			REQUIRE(!auto_test(ab, "a"));
			REQUIRE(!auto_test(ab, "b"));
			REQUIRE(!auto_test(ab, ""));
			REQUIRE(!auto_test(ab, "aaa"));
			REQUIRE(!auto_test(ab, "bbb"));
			REQUIRE(!auto_test(ab, "ba"));
			auto_destroy(ab);
		}
		auto_destroy(b);
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
		THEN("the concatenation accepts words of 3 arbitrary letters followed by 'a's /...a+/"){
			State a3aa = auto_concat(a3, aa);
			REQUIRE(auto_test(a3aa, "aaaa"));
			REQUIRE(auto_test(a3aa, "abba"));
			REQUIRE(auto_test(a3aa, "???aaaaaaaaa"));
			REQUIRE(auto_test(a3aa, "+-aaaaa"));
			REQUIRE(!auto_test(a3aa, "aaa"));
			REQUIRE(!auto_test(a3aa, "a"));
			REQUIRE(!auto_test(a3aa, "grrrrrrr"));
			REQUIRE(!auto_test(a3aa, "grrraaaaaa"));
			auto_destroy(a3aa);
		}
		THEN("the concatenation accepts words of 'a's followed by 3 arbitrary letters /a+.../"){
			State aaa3 = auto_concat(aa, a3);
			REQUIRE(auto_test(aaa3, "aaaa"));
			REQUIRE(auto_test(aaa3, "abba"));
			REQUIRE(auto_test(aaa3, "aaaaaaaabba"));
			REQUIRE(!auto_test(aaa3, "aaa"));
			REQUIRE(!auto_test(aaa3, "a"));
			REQUIRE(!auto_test(aaa3, "grrrrrrr"));
			REQUIRE(!auto_test(aaa3, "aaaaaarrrrq"));
			auto_destroy(aaa3);
		}
		THEN("/(...)*/ accepts words of length multiple of 3 and empty too"){
			Automaton a33 = auto_kleene(a3);
			REQUIRE(auto_test(a33, "asf"));
			REQUIRE(auto_test(a33, "aaa"));
			REQUIRE(auto_test(a33, "   "));
			REQUIRE(auto_test(a33, "aaa   "));
			REQUIRE(auto_test(a33, "aaa   asf"));
			REQUIRE(!auto_test(a33, "    "));
			REQUIRE(!auto_test(a33, "  "));
			REQUIRE(auto_test(a33, ""));
			auto_destroy(a33);
		}
		THEN("/(...)+/ accepts words of length multiple of 3"){
			Automaton a33 = auto_kleeneplus(a3);
			REQUIRE(auto_test(a33, "asf"));
			REQUIRE(auto_test(a33, "aaa"));
			REQUIRE(auto_test(a33, "   "));
			REQUIRE(auto_test(a33, "aaa   "));
			REQUIRE(auto_test(a33, "aaa   asf"));
			REQUIRE(!auto_test(a33, "    "));
			REQUIRE(!auto_test(a33, "  "));
			REQUIRE(!auto_test(a33, ""));
			auto_destroy(a33);
		}
		auto_destroy(aa);
		auto_destroy(a3);
	}
}

extern "C" {
#include <parc24/options.h>
#include <parc24/patterns.h>
}

SCENARIO("standard patterns", "[patterns]"){
	struct parc_options opts = parc_options_default;
	PatternCompiler compiler = patcomp_new();
	if(!compiler) FAIL("failed to initialize compiler");
	GIVEN("/a/"){
		THEN("pattern compiles"){
			IfElse_T(pattern_compile(compiler, "a", opts), p, {
				AND_THEN("pattern matches"){
					REQUIRE(pattern_test(p, "a"));
					REQUIRE(!pattern_test(p, "b"));
					REQUIRE(!pattern_test(p, "c"));
					REQUIRE(!pattern_test(p, ""));
					REQUIRE(!pattern_test(p, "dvfsvgfdrf"));
				}
				pattern_destroy(p);
			}, err, { FAIL_FMT("Pattern compilation failed: %s", err.s); });
		}
	}
	GIVEN("/abc/"){
		THEN("pattern compiles"){
			IfElse_T(pattern_compile(compiler, "abc", opts), p, {
				AND_THEN("pattern matches"){
					REQUIRE(pattern_test(p, "abc"));
					REQUIRE(!pattern_test(p, "akc"));
					REQUIRE(!pattern_test(p, "azc"));
					REQUIRE(!pattern_test(p, "a"));
					REQUIRE(!pattern_test(p, "b"));
					REQUIRE(!pattern_test(p, "c"));
					REQUIRE(!pattern_test(p, ""));
					REQUIRE(!pattern_test(p, "dvfsvgfdrf"));
				}
				pattern_destroy(p);
			}, err, { FAIL_FMT("Pattern compilation failed: %s", err.s); });
		}
	}
	GIVEN("/a?c/"){
		THEN("pattern compiles"){
			IfElse_T(pattern_compile(compiler, "a?c", opts), p, {
				AND_THEN("pattern matches"){
					REQUIRE(pattern_test(p, "abc"));
					REQUIRE(pattern_test(p, "akc"));
					REQUIRE(pattern_test(p, "azc"));
					REQUIRE(!pattern_test(p, "a"));
					REQUIRE(!pattern_test(p, "b"));
					REQUIRE(!pattern_test(p, "c"));
					REQUIRE(!pattern_test(p, ""));
					REQUIRE(!pattern_test(p, "dvfsvgfdrf"));
					REQUIRE(!pattern_test(p, "a/c"));
				}
				pattern_destroy(p);
			}, err, { FAIL_FMT("Pattern compilation failed: %s", err.s); });
		}
	}
	GIVEN("/a*c/"){
		THEN("pattern compiles"){
			IfElse_T(pattern_compile(compiler, "a*c", opts), p, {
				AND_THEN("pattern matches"){
					REQUIRE(pattern_test(p, "abc"));
					REQUIRE(pattern_test(p, "akc"));
					REQUIRE(pattern_test(p, "azc"));
					REQUIRE(pattern_test(p, "ac"));
					REQUIRE(pattern_test(p, "aaaaaacccccc"));
					REQUIRE(pattern_test(p, "a oh hello there ## nyeh! c"));
					REQUIRE(!pattern_test(p, "a"));
					REQUIRE(!pattern_test(p, "b"));
					REQUIRE(!pattern_test(p, "c"));
					REQUIRE(!pattern_test(p, ""));
					REQUIRE(!pattern_test(p, "dvfsvgfdrf"));
					REQUIRE(!pattern_test(p, "a/c"));
					REQUIRE(!pattern_test(p, "a old mcdonald / found mcdonalds or smthn c"));
				}
				pattern_destroy(p);
			}, err, { FAIL_FMT("Pattern compilation failed: %s", err.s); });
		}
	}
	GIVEN("/a[a-z]c/"){
		THEN("pattern compiles"){
			IfElse_T(pattern_compile(compiler, "a[a-z]c", opts), p, {
				AND_THEN("pattern matches"){
					REQUIRE(pattern_test(p, "abc"));
					REQUIRE(pattern_test(p, "akc"));
					REQUIRE(pattern_test(p, "azc"));
					REQUIRE(!pattern_test(p, "aZc"));
					REQUIRE(!pattern_test(p, "aEc"));
					REQUIRE(!pattern_test(p, "a-c"));
					REQUIRE(!pattern_test(p, "a.c"));
					REQUIRE(!pattern_test(p, "a0c"));
					REQUIRE(!pattern_test(p, "a"));
					REQUIRE(!pattern_test(p, "b"));
					REQUIRE(!pattern_test(p, "c"));
					REQUIRE(!pattern_test(p, ""));
					REQUIRE(!pattern_test(p, "dvfsvgfdrf"));
					REQUIRE(!pattern_test(p, "a/c"));
				}
				pattern_destroy(p);
			}, err, { FAIL_FMT("Pattern compilation failed: %s", err.s); });
		}
	}
	GIVEN("/a[^a-z]c/"){
		THEN("pattern compiles"){
			IfElse_T(pattern_compile(compiler, "a[^a-z]c", opts), p, {
				AND_THEN("pattern matches"){
					REQUIRE(!pattern_test(p, "abc"));
					REQUIRE(!pattern_test(p, "akc"));
					REQUIRE(!pattern_test(p, "azc"));
					REQUIRE(pattern_test(p, "aZc"));
					REQUIRE(pattern_test(p, "aEc"));
					REQUIRE(pattern_test(p, "a-c"));
					REQUIRE(pattern_test(p, "a.c"));
					REQUIRE(pattern_test(p, "a0c"));
					REQUIRE(pattern_test(p, "a/c"));
					REQUIRE(!pattern_test(p, "a"));
					REQUIRE(!pattern_test(p, "b"));
					REQUIRE(!pattern_test(p, "c"));
					REQUIRE(!pattern_test(p, ""));
					REQUIRE(!pattern_test(p, "dvfsvgfdrf"));
				}
				pattern_destroy(p);
			}, err, { FAIL_FMT("Pattern compilation failed: %s", err.s); });
		}
	}
	GIVEN("/a[[:alpha:]]c/"){
		THEN("pattern compiles"){
			IfElse_T(pattern_compile(compiler, "a[[:alpha:]]c", opts), p, {
				AND_THEN("pattern matches"){
					REQUIRE(pattern_test(p, "abc"));
					REQUIRE(pattern_test(p, "akc"));
					REQUIRE(pattern_test(p, "azc"));
					REQUIRE(pattern_test(p, "aZc"));
					REQUIRE(pattern_test(p, "aEc"));
					REQUIRE(!pattern_test(p, "a-c"));
					REQUIRE(!pattern_test(p, "a.c"));
					REQUIRE(!pattern_test(p, "a0c"));
					REQUIRE(!pattern_test(p, "a"));
					REQUIRE(!pattern_test(p, "b"));
					REQUIRE(!pattern_test(p, "c"));
					REQUIRE(!pattern_test(p, ""));
					REQUIRE(!pattern_test(p, "dvfsvgfdrf"));
					REQUIRE(!pattern_test(p, "a/c"));
				}
				pattern_destroy(p);
			}, err, { FAIL_FMT("Pattern compilation failed: %s", err.s); });
		}
	}
	patcomp_destroy(compiler);
}
