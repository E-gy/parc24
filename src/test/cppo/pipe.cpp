#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <cppo.h>
#include <unistd.h>
#include <string.h>
}

#define STRS 256

SCENARIO("pipe does the piping", "[pipe][cppo]"){
	auto strb = GENERATE(chunk(STRS, take(STRS*8, random(' ', '~'))));
	strb.push_back('\0');
	string str = strb.data();
	size_t strl = strlen(str);
	GIVEN("a string to pipe"){
		WHEN("a pipe can be opened"){
			IfElse_T(pipe_new(), pipe, {
				THEN("data can be written to the pipe"){
					size_t wr = 0;
					while(wr < strl){
						int w = write(pipe.write, str+wr, strl-wr);
						if(w <= 0) FAIL("Write to pipe failed");
						wr += w;
					}
					if(close(pipe.write) < 0) FAIL("Close write end failed");
					AND_THEN("data can be read back"){
						char buff[STRS+1] = {'\0'};
						int rr = 0;
						while(true){
							int r = read(pipe.read, buff+rr, strl-rr);
							if(r < 0) FAIL("Read from pipe failed");
							if(r == 0) break;
							rr += r;
						}
						if(close(pipe.read) < 0) FAIL("Close read end failed");
						REQUIRE_THAT(buff, Equals(str));
					}
				}
			}, err, { FAIL_FMT("Pipe creation failed with error - %s", err.s); });
		}
	}
}
