#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <cppo.h>
#include <util/null.h>
}

SCENARIO("join arguments", "[args][util][cppo]"){
	WHEN("given nothing"){
		string_mut cmd = exe_args_join(null);
		THEN("error with nothing"){
			REQUIRE(cmd == (char*)null);
		}
		free(cmd);
	}
	WHEN("given empty"){
		string args[] = {null};
		string_mut cmd = exe_args_join(args);
		THEN("result is empty"){
			REQUIRE_THAT(cmd, Equals(""));
		}
		free(cmd);
	}
	WHEN("given 1 arg"){
		string args[] = {"hello", null};
		string_mut cmd = exe_args_join(args);
		THEN("result that arg"){
			REQUIRE_THAT(cmd, Equals("hello"));
		}
		free(cmd);
	}
	WHEN("given 2 args"){
		string args[] = {"hello", "world", null};
		string_mut cmd = exe_args_join(args);
		THEN("they are space joined"){
			REQUIRE_THAT(cmd, Equals("hello world"));
		}
		free(cmd);
	}
	WHEN("argument contains space"){
		string args[] = {"hello world", null};
		string_mut cmd = exe_args_join(args);
		THEN("it is quoted"){
			REQUIRE_THAT(cmd, Equals("'hello world'"));
		}
		free(cmd);
	}
	WHEN("argument contains double quote"){
		string args[] = {"hello\"world", null};
		string_mut cmd = exe_args_join(args);
		THEN("it is quoted"){
			REQUIRE_THAT(cmd, Equals("'hello\"world'"));
		}
		free(cmd);
	}
	WHEN("argument contains single quote"){
		string args[] = {"hello'world", null};
		string_mut cmd = exe_args_join(args);
		THEN("it is quoted and escaped"){
			#ifdef _WIN32
			REQUIRE_THAT(cmd, Equals("'hello''world'"));
			#else
			REQUIRE_THAT(cmd, Equals("'hello'\\''world'"));
			#endif
		}
		free(cmd);
	}
	WHEN("multiple weird arguments"){
		string args[] = {"hello", "th'e're", "\"general\" Kenobi", "!", null};
		string_mut cmd = exe_args_join(args);
		THEN("everything is ookay"){
			REQUIRE_THAT(cmd, Equals(
			#ifdef _WIN32
			"hello 'th''e''re' '\"general\" Kenobi' !"
			#else
			"hello 'th'\\''e'\\''re' '\"general\" Kenobi' !"
			#endif
			));
		}
		free(cmd);
	}
}
