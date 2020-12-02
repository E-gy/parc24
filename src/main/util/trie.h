/**
 * @file trie.h
 * Type safe Trie
 * 
 * To be _strictly_ used only inside implementation files.
 * The types must _not_ be exposed.
 * The functions _must_ be abstracted over.
 * 
 * All that to guarantee type safety*** with 0 casts.
 * 
 * The generic Trie only support pointed-to types. Automatic destruction included ;)
 * Usage:
 * 1. define `_TRIE_IMPL_T` to your pointy type
 * 2. include the header
 * 
 * PS: technically any type <= sizeof(void*) can be used.
 * I hold 0 responsibility if you do not use a pointy type and fail to guarantee the above for:
 * - RSODs
 * - rainbows coming out of your CD bay
 * - climate chage
 * - or any other cataclismic event
 * 
 */

#pragma once

#ifndef _TRIE_IMPL_T
#define _TRIE_IMPL_T void*
#endif

#include <ptypes.h>

typedef _TRIE_IMPL_T Trie_V;
typedef void (*Trie_V_Destructor)(Trie_V);
typedef Trie_V (*Trie_V_Copy)(Trie_V);

struct trie;
typedef struct trie* Trie;

/**
 * @param destructor @ref 
 * @return @produces Trie 
 */
Trie trie_new(Trie_V_Destructor destructor);

/**
 * @param t @consumes
 */
void trie_destroy(Trie t);

/**
 * @param store @ref
 * @return @produces Trie 
 */
Trie trie_clone(Trie store, Trie_V_Copy cloner);

/**
 * @param t @ref 
 * @param key @ref
 * @return @ref Trie_V 
 */
Trie_V trie_get(Trie t, string key);

/**
 * @param t @refmut 
 * @param key @ref
 * @param val @consumes
 * @return Result 
 */
Result trie_add(Trie t, string key, Trie_V val);

/**
 * @param store @ref
 * @param key @ref
 * @return Result 
 */
Result trie_remove(Trie store, string key);

