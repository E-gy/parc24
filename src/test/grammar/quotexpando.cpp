#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <grammar/quotexpando.h>
#include <util/null.h>
}

SCENARIO("word capture", "[word][lexer]"){
	GIVEN("empty string") THEN("no capture") REQUIRE(capture_word("") == nullstr);
	GIVEN("plain simple word followed by weird things"){
		auto word = GENERATE("birb", "birb   ", "birb		", "birb;", "birb(", "birb)", "birb\n", "birb\r\n", "birb<", "birb>");
		CAPTURE(word);
		THEN("capture until break"){
			auto capt = capture_word(word);
			REQUIRE(capt != nullstr);
			REQUIRE(capt-word == 4);
		}
	}
	GIVEN("plain word with unspecial chars"){
		auto word = GENERATE("125 <$>", "potato <$>", "duk-m <$>", "question_mark,and.most-punctuation?is*not^meta@actually <$>");
		CAPTURE(word);
		THEN("capture them all"){
			auto capt = capture_word(word);
			REQUIRE(capt != nullstr);
			REQUIRE_THAT(capt, Equals(" <$>"));
		}
	}
	GIVEN("escaped specials"){
		auto word = GENERATE("birb\\  <$>", "birb\\ bro <$>", "birb\\	 <$>", "birb\\$ <$>", "birb\\; <$>", "birb\\$\\$ <$>", "birb\\( <$>", "birb\\) <$>", "birb\\\n <$>", "birb\\\r\\\n <$>", "birb\\< <$>", "birb\\> <$>");
		CAPTURE(word);
		THEN("capture them all"){
			auto capt = capture_word(word);
			REQUIRE(capt != nullstr);
			REQUIRE_THAT(capt, Equals(" <$>"));
		}
	}
	GIVEN("single quoted string"){
		auto word = GENERATE("'' <$>", "'potato' <$>", "'pum tam dum ""woo' <$>");
		CAPTURE(word);
		THEN("capture them all"){
			auto capt = capture_word(word);
			REQUIRE(capt != nullstr);
			REQUIRE_THAT(capt, Equals(" <$>"));
		}
	}
	GIVEN("double quoted string"){
		auto word = GENERATE("\"\" <$>", "\"potato\" <$>", "\"pum tam dum \\\"\\\"woo\" <$>");
		CAPTURE(word);
		THEN("capture them all"){
			auto capt = capture_word(word);
			REQUIRE(capt != nullstr);
			REQUIRE_THAT(capt, Equals(" <$>"));
		}
	}
	GIVEN("escape hell"){
		REQUIRE_THAT(capture_word("\\a <$>"), Equals(" <$>"));
		REQUIRE_THAT(capture_word("\\& <$>"), Equals(" <$>"));
		REQUIRE_THAT(capture_word("\\\\& <$>"), Equals("& <$>"));
		REQUIRE_THAT(capture_word("\\\\\\& <$>"), Equals(" <$>"));
		REQUIRE_THAT(capture_word("\\\\\\\\& <$>"), Equals("& <$>"));
	}
}
