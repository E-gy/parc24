#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <cppo/util.h>
#include <util/null.h>
}

SCENARIO("join arguments", "[args][util][cppo]"){
	GIVEN("nothing"){
		string_mut cmd = exe_args_join(null, false);
		THEN("error with nothing"){
			REQUIRE(cmd == (char*)null);
		}
		free(cmd);
	}
	GIVEN("empty"){
		string args[] = {null};
		string_mut cmd = exe_args_join(args, false);
		THEN("result is empty"){
			REQUIRE_THAT(cmd, Equals(""));
		}
		free(cmd);
	}
	GIVEN("1 arg"){
		string args[] = {"hello", null};
		string_mut cmd = exe_args_join(args, false);
		THEN("result that arg"){
			REQUIRE_THAT(cmd, Equals("hello"));
		}
		free(cmd);
	}
	GIVEN("2 args"){
		string args[] = {"hello", "world", null};
		string_mut cmd = exe_args_join(args, false);
		THEN("they are space joined"){
			REQUIRE_THAT(cmd, Equals("hello world"));
		}
		free(cmd);
	}
	GIVEN("argument containing space"){
		string args[] = {"hello world", null};
		string_mut cmd = exe_args_join(args, false);
		THEN("it is quoted"){
			REQUIRE_THAT(cmd, Equals("'hello world'"));
		}
		free(cmd);
	}
	GIVEN("argument containing double quote"){
		string args[] = {"hello\"world", null};
		string_mut cmd = exe_args_join(args, false);
		THEN("it is quoted"){
			REQUIRE_THAT(cmd, Equals("'hello\"world'"));
		}
		free(cmd);
	}
	GIVEN("argument containing single quote"){
		string args[] = {"hello'world", null};
		string_mut cmd = exe_args_join(args, false);
		THEN("it is quoted and escaped"){
			// #ifdef _WIN32
			// REQUIRE_THAT(cmd, Equals("'hello''world'"));
			// #endif
			REQUIRE_THAT(cmd, Equals("'hello'\\''world'"));
		}
		free(cmd);
	}
	GIVEN("multiple weird arguments"){
		string args[] = {"hello", "th'e're", "\"general\" Kenobi", "!", null};
		string_mut cmd = exe_args_join(args, false);
		THEN("everything is ookay"){
			REQUIRE_THAT(cmd, Equals(
			// #ifdef _WIN32
			// "hello 'th''e''re' '\"general\" Kenobi' !"
			// #else
			"hello 'th'\\''e'\\''re' '\"general\" Kenobi' !"
			// #endif
			));
		}
		free(cmd);
	}
}
