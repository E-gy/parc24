#include <catch2ext.hpp>
#include <sstream>
using Catch::Matchers::Equals;

extern "C" {
#include <tihs/opts.h>
#include <util/null.h>
#include <parc24/ioslog.h>
#include <parc24/context.h>
#include <tihs/opts.h>
#include <tihs/exe.h>
#include <parc24/pars.h>
#include <builtins/ccmds.h>
#include <cppo.h>
#include <cppo/parallels.h>
}

struct parctesting {
	fd_t stdin_w;
	fd_t stdout_r;
	fd_t stderr_r;
	struct parcontext ctxt;
};

#define initectxt() __extension__({ \
		struct parc_options parcopts = parc_options_default; \
		auto p0r = pipe_new(), p1r = pipe_new(), p2r = pipe_new(); \
		if(!IsOk_T(p0r) || !IsOk_T(p1r) || !IsOk_T(p2r)) FAIL("failed to create pipes"); \
		IOsStack ios = iosstack_new(); \
		if(!ios) FAIL("failed to create IO stack"); \
		iosstack_raw_set(ios, IOSTREAM_STD_IN, p0r.r.ok.read); \
		iosstack_raw_set(ios, IOSTREAM_STD_OUT, p1r.r.ok.write); \
		iosstack_raw_set(ios, IOSTREAM_STD_ERR, p2r.r.ok.write); \
		CCMDStore ccmds = ccmdstore_new(); \
		if(!ccmds) FAIL("failed to ccmds store"); \
		ccmdstore_set(ccmds, "exit", cmd_exit); \
		ccmdstore_set(ccmds, "break", cmd_break); \
		ccmdstore_set(ccmds, "continue", cmd_continue); \
		ccmdstore_set(ccmds, "echo", cmd_echo); \
		ccmdstore_set(ccmds, "shopt", cmd_shopt); \
		string_mut noargs[] = {null}; \
		auto vars = varstore_new(); \
		(struct parctesting){ p0r.r.ok.write, p1r.r.ok.read, p2r.r.ok.read, { \
			vars, vars, funcstore_new(), ccmds, aliastore_new(), ios, patcomp_new(), \
			"parc", noargs, 0, false, &parcopts, \
			parcer_defolt_new() \
		}}; \
	})

#define destrctxt(context) do { \
		patcomp_destroy(context.ctxt.patcomp); \
		aliastore_destroy(context.ctxt.aliases); \
		ccmdstore_destroy(context.ctxt.ccmds); \
		funcstore_destroy(context.ctxt.funcs); \
		varstore_destroy(context.ctxt.vars); \
		parser_destroy(context.ctxt.parcer); \
		iosstack_destroy(context.ctxt.ios); \
	} while(0)

SCENARIO("variables assignments", "[variables][full stack][parc]"){
	auto ctxt = initectxt();
	string_mut output;
	auto outread = parallels_readstr(ctxt.stdout_r, &output);
	if(!IsOk_T(outread)) FAIL("output read setup failed");
	WHEN("assigning a value"){
		auto exer = tihs_exestr("VAR=apache", &ctxt.ctxt);
		REQUIRE(IsOk_T(exer));
		THEN("the value is stored"){
			REQUIRE_THAT(varstore_get(ctxt.ctxt.vars, "VAR"), Equals("apache"));
		}
	}
	WHEN("assigning an empty value"){
		auto exer = tihs_exestr("VAR=", &ctxt.ctxt);
		REQUIRE(IsOk_T(exer));
		THEN("the empty is stored"){
			REQUIRE_THAT(varstore_get(ctxt.ctxt.vars, "VAR"), Equals(""));
		}
	}
	WHEN("assigning multiple values"){
		auto v1 = GENERATE("grr", "woof", "", "kkk");
		auto v2 = GENERATE("grr", "woof", "", "kkk");
		auto v3 = GENERATE("grr", "woof", "", "kkk");
		std::ostringstream strs;
		strs << "V1=" << v1 << " V2=" << v2 << " V3=" << v3;
		std::string str = strs.str();
		auto exer = tihs_exestr(str.c_str(), &ctxt.ctxt);
		THEN("the values are stored"){
			REQUIRE_THAT(varstore_get(ctxt.ctxt.vars, "V1"), Equals(v1));
			REQUIRE_THAT(varstore_get(ctxt.ctxt.vars, "V2"), Equals(v2));
			REQUIRE_THAT(varstore_get(ctxt.ctxt.vars, "V3"), Equals(v3));
		}
	}
	destrctxt(ctxt); //we're done executing things - flush all outputs so we can check them
	if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
	REQUIRE_THAT(output, Equals(""));
	free(output);
}

SCENARIO("echo does the echo", "[echo][full stack][parc]"){
	auto ctxt = initectxt();
	string_mut output;
	auto outread = parallels_readstr(ctxt.stdout_r, &output);
	if(!IsOk_T(outread)) FAIL("output read setup failed");
	GIVEN("initialized context"){
		AND_GIVEN("constant to say"){
			auto exer = tihs_exestr("echo hello", &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt); //we're done executing things - flush all outputs so we can check them
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals("hello\n"));
		}
		AND_GIVEN("-n constant to say"){
			auto exer = tihs_exestr("echo -n hello", &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt); //we're done executing things - flush all outputs so we can check them
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals("hello"));
		}
		AND_GIVEN("constant to say and a comment"){
			auto exer = tihs_exestr("echo hello #recent friend", &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt); //we're done executing things - flush all outputs so we can check them
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals("hello\n"));
		}
	}
	free(output);
}

SCENARIO("expansions", "[expando][full stack][parc]"){
	auto ctxt = initectxt();
	string_mut output;
	auto outread = parallels_readstr(ctxt.stdout_r, &output);
	if(!IsOk_T(outread)) FAIL("output read setup failed");
	GIVEN("word plain and simple"){
		THEN("it is kept as is"){
			auto exer = tihs_exestr("echo hello", &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt);
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals("hello\n"));
		}
	}
	GIVEN("escaped sequences"){
		auto exeo = GENERATE(std::tuple<string, string>{"echo -n he\\llo\\ there", "hello there"}, std::tuple<string, string>{"echo -n he\\'llo", "he'llo"}, std::tuple<string, string>{"echo -n he\\\"llo", "he\"llo"}, std::tuple<string, string>{"echo -n he\\$llo", "he$llo"}, std::tuple<string, string>{"echo -n he\\\\llo", "he\\llo"});
		CAPTURE(std::get<0>(exeo));
		THEN("escaped sequences are escaped"){
			auto exer = tihs_exestr(std::get<0>(exeo), &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt);
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals(std::get<1>(exeo)));
		}
	}
	GIVEN("single quotes"){
		auto exeo = GENERATE(std::tuple<string, string>{"echo -n 'hello there'", "hello there"}, std::tuple<string, string>{"echo -n '\"hello\": \\$ there'", "\"hello\": \\$ there"}, std::tuple<string, string>{"echo -n 'hello there'\\''onari gong'", "hello there'onari gong"});
		CAPTURE(std::get<0>(exeo));
		THEN("contents are preserved"){
			auto exer = tihs_exestr(std::get<0>(exeo), &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt);
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals(std::get<1>(exeo)));
		}
	}
	GIVEN("variable expansion"){
		auto expw = GENERATE(false, true);
		auto v = GENERATE("woof", "meow meow i'm a cow", "a:b:c e:f j:k");
		CAPTURE(v);
		REQUIRE(IsOk(varstore_add(ctxt.ctxt.vars, "abc", v)));
		THEN("contents are inserted as is"){
			auto exer = tihs_exestr(expw ? "echo -n ${abc}" :"echo -n $abc", &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt);
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals(v));
		}
	}
	GIVEN("double quotes"){
		auto exeo = GENERATE(std::tuple<string, string>{"echo -n \"hello there\"", "hello there"}, std::tuple<string, string>{"echo -n \"hello\\\\ there\\$\"", "hello\\ there$"}, std::tuple<string, string>{"echo -n \"\\\"hello\\\" \\`\\` there\"", "\"hello\" `` there"});
		CAPTURE(std::get<0>(exeo));
		THEN("quoting rules are respected, incl. escape sequences"){
			auto exer = tihs_exestr(std::get<0>(exeo), &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt);
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals(std::get<1>(exeo)));
		}
	}
	GIVEN("command to expand"){
		auto exeo = GENERATE(std::tuple<string, string>{"echo -n `echo -n hello there`", "hello there"}, std::tuple<string, string>{"echo -n $(echo -n hello there)", "hello there"}, std::tuple<string, string>{"echo -n $(echo -n `echo -n hello` $(echo -n there))", "hello there"});
		CAPTURE(std::get<0>(exeo));
		THEN("output is captured and inserted"){
			auto exer = tihs_exestr(std::get<0>(exeo), &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt);
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals(std::get<1>(exeo)));
		}
	}
	free(output);
}
