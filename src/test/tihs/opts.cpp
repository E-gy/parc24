#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <tihs/opts.h>
#include <util/null.h>
#include <parc24/ioslog.h>
#include <parc24/iosstack.h>
}

SCENARIO("deciding where to read from", "[opts][parse][tihs]"){
	IOsStack ios = parcio_new_fromstd();
	WHEN("nada args"){
		TihsOptsParseResult res = tihsopts_parse_caste(null, ios);
		THEN("error"){
			REQUIRE(!IsOk_T(res));
		}
	}
	WHEN("no args"){
		string args[] = {null};
		THEN("parsing ok"){
			IfElse_T(tihsopts_parse_caste(args, ios), opts, {
				AND_THEN("read from stdin"){
					REQUIRE(!opts.commandstr);
					REQUIRE(!opts.commandfile);
					REQUIRE(opts.args == args);
					REQUIRE(tihsopts_from_stdin(opts));
				}
			}, err, {FAIL_FMT("Parsing args failed - %s", err.s);});
		}
	}
	WHEN("single arg"){
		string args[] = {"file.script", null};
		THEN("parsing ok"){
			IfElse_T(tihsopts_parse_caste(args, ios), opts, {
				AND_THEN("read from file and positional args start at the file"){
					REQUIRE(!opts.commandstr);
					REQUIRE_THAT(opts.commandfile, Equals(args[0]));
					REQUIRE(opts.args == args);
					REQUIRE(!tihsopts_from_stdin(opts));
				}
			}, err, {FAIL_FMT("Parsing args failed - %s", err.s);});
		}
	}
	WHEN("explicit stdin"){
		string args[] = {"-s", null};
		THEN("parsing ok"){
			IfElse_T(tihsopts_parse_caste(args, ios), opts, {
				AND_THEN("read from stdin"){
					REQUIRE(!opts.commandstr);
					REQUIRE(!opts.commandfile);
					REQUIRE(opts.args == args+1);
					REQUIRE(tihsopts_from_stdin(opts));
				}
			}, err, {FAIL_FMT("Parsing args failed - %s", err.s);});
		}
	}
	WHEN("read from string"){
		WHEN("args end too early"){
			string args[] = {"-c", null};
			TihsOptsParseResult res = tihsopts_parse_caste(args, ios);
			THEN("error"){
				REQUIRE(!IsOk_T(res));
			}
		}
		WHEN("it's the last arg"){
			string args[] = {"-c", "echo 'dub dub'", null};
			THEN("parsing ok"){
				IfElse_T(tihsopts_parse_caste(args, ios), opts, {
					AND_THEN("read from the string"){
						REQUIRE_THAT(opts.commandstr, Equals(args[1]));
						REQUIRE(!opts.commandfile);
						REQUIRE(opts.args == args+2);
						REQUIRE(!tihsopts_from_stdin(opts));
					}
				}, err, {FAIL_FMT("Parsing args failed - %s", err.s);});
			}
		}
		WHEN("it isn't the last arg"){
			string args[] = {"-c", "echo 'dub dub'", "-s", "-c", "ping pong", null};
			THEN("parsing ok"){
				IfElse_T(tihsopts_parse_caste(args, ios), opts, {
					AND_THEN("read from the string and everything after it is not parsed"){
						REQUIRE_THAT(opts.commandstr, Equals(args[1]));
						REQUIRE(!opts.commandfile);
						REQUIRE(opts.args == args+2);
						REQUIRE(!tihsopts_from_stdin(opts));
					}
				}, err, {FAIL_FMT("Parsing args failed - %s", err.s);});
			}
		}
	}
	iosstack_destroy(ios);
}
