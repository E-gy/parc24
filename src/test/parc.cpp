#include <catch2ext.hpp>
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

struct parc_options parcodef(){
	return parc_options_default;
}
}

struct parctesting {
	fd_t stdin_w;
	fd_t stdout_r;
	fd_t stderr_r;
	struct parcontext ctxt;
};

#define initectxt() __extension__({ \
		struct parc_options parcopts = parcodef(); \
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
		(struct parctesting){ p0r.r.ok.write, p1r.r.ok.read, p2r.r.ok.read, { \
			varstore_new(), funcstore_new(), ccmds, aliastore_new(), ios, \
			"parc", noargs, 0, false, &parcopts, \
			parcer_defolt_new() \
		}}; \
	})

#define destrctxt(context) do { \
		aliastore_destroy(context.ctxt.aliases); \
		ccmdstore_destroy(context.ctxt.ccmds); \
		funcstore_destroy(context.ctxt.funcs); \
		varstore_destroy(context.ctxt.vars); \
		parser_destroy(context.ctxt.parcer); \
		iosstack_destroy(context.ctxt.ios); \
	} while(0)

SCENARIO("echo does the echo", "[echo][full stack][parc]"){
	auto ctxt = initectxt();
	GIVEN("initialized context"){
		AND_GIVEN("constant to say"){
			string_mut output;
			auto outread = parallels_readstr(ctxt.stdout_r, &output);
			if(!IsOk_T(outread)) FAIL("output read setup failed");
			auto exer = tihs_exestr("echo hello", &ctxt.ctxt);
			REQUIRE(IsOk_T(exer));
			destrctxt(ctxt); //we're done executing things - flush all outputs so we can check them
			if(!IsOk_T(exethread_waitretcode(outread.r.ok))) FAIL("output read wait failed");
			REQUIRE_THAT(output, Equals("hello\n"));
			free(output);
		}
	}
}
