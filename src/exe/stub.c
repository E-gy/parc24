#include <stdlib.h>
#include <ptypes.h>

#include <parc24/io.h>
#include <tihs/opts.h>

int main(int argc, argsarr args){
	const ParC24IO io = parc24io_fromstd();
	TihsOptsParseResult optpars = tihsopts_parse_caste(args+1, io);
	IfError_T(optpars, err, {
		io.log(LL_CRITICAL, "Failed to parse arguments - %s", err.s);
		return 1;
	});
	struct tihsopts opts = optpars.r.ok;
}
