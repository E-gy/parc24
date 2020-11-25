#include <catch2/catch.hpp>
using Catch::Matchers::Equals;

extern "C"{
	#include <lib.h>
	#include <stdlib.h>
}

SCENARIO("say hi to someone", "[sayhi][sample]"){
	WHEN("say hi to no one"){
		char* hi = sayhi(NULL);
		REQUIRE(!!hi);
		THEN("we say hi to the one and only!"){
			REQUIRE_THAT(hi, Equals("Hello there, General Kenobi!~~\\(-v-)/~~|"));
		}
		free(hi);
	}
	WHEN("say hi to blank"){
		char* hi = sayhi("");
		REQUIRE(!!hi);
		THEN("we say hi to the one and only!"){
			REQUIRE_THAT(hi, Equals("Hello there, General Kenobi!~~\\(-v-)/~~|"));
		}
		free(hi);
	}
	WHEN("say hi to some guy"){
		char* hi = sayhi("some guy");
		REQUIRE(!!hi);
		THEN("we say hi to that one guy"){
			REQUIRE_THAT(hi, Equals("Hello there, some guy!~~\\(-v-)/~~|"));
		}
		free(hi);
	}
}
