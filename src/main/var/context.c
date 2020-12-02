#include <parc24/context.h>

string parcontext_getunivar(ParContext c, string v){
	return varstore_get(c->vars, v);
}
