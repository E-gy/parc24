#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <cppo.h>
#include <cppo/parallels.h>
#include <util/caste.h>
}

#define STRS 262144

SCENARIO("with parallels we can pipe BIG!", "[parallels][pipe][cppo]"){
	auto strb = GENERATE(chunk(STRS, take(STRS, random(' ', '~'))));
	strb.push_back('\0');
	const string str = strb.data();
	const size_t strl = strb.size();
	GIVEN("megabytes of data to pipe"){
		WHEN("a pipe can be opened"){
			IfElse_T(pipe_new(), pipe, {
				THEN("parallels do their job"){
					string_mut result;
					auto wr = parallels_writestr(pipe.write, str, true);
					auto rr = parallels_readstr(pipe.read, &result);
					REQUIRE(IsOk_T(wr));
					REQUIRE(IsOk_T(rr));
					auto rrw = exethread_waitretcode(rr.r.ok);
					REQUIRE(IsOk_T(rrw));
					REQUIRE(rrw.r.ok == 0);
					REQUIRE_THAT(result, Equals(str));
					free(result);
				}
			}, err, { FAIL_FMT("Pipe creation failed with error - %s", err.s); });
		}
	}
}
