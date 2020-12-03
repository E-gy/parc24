#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <cppo.h>
#include <cppo/parallels.h>
#include <util/caste.h>
#include <pthread.h>
}

#define STRS 8388608

SCENARIO("with parallels we can pipe BIG!", "[parallels][pipe][cppo]"){
	auto strb = GENERATE(chunk(STRS, take(STRS*4, random(' ', '~'))));
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
					void* rrr;
					REQUIRE(pthread_join(rr.r.ok, &rrr) == 0);
					int rs = ptr2int(rrr);
					REQUIRE(rs == 0);
					REQUIRE_THAT(result, Equals(str));
					free(result);
				}
			}, err, { FAIL_FMT("Pipe creation failed with error - %s", err.s); });
		}
	}
}
