#pragma once

/**
 * @file caste.h
 * 🌈🦄✨
 * it's a magical casting device you should use it with care
 * ~~or zillions of universes shall perish in eternal despair~~
 */

union ptcaster {
	int i;
	unsigned int ui;
	long long l;
	unsigned long long ul;
	void* t;
	const void* ct;
	void (*f)(void);
};

#define caste(val, is, os) ((union ptcaster){.is = val}).os
#define ptr2ptr(val) caste(val, t, t)
#define cpt2ptr(val) caste(val, ct, t)
#define ptr2fun(val) caste(val, t, f)
#define fun2ptr(val) caste(val, f, t)
#define int2ptr(val) caste(val, i, t)
#define ptr2int(val) caste(val, t, i)
#define ull2ptr(val) caste(val, ul, t)
#define ptr2ull(val) caste(val, t, ul)
