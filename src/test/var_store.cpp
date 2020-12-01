#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <parc24/var_store.h>
#include <util/null.h>
}

SCENARIO("storing, getting, and removing variables from the store", "[variables store][parc24]"){
	WHEN("there is nothing"){
		VarStore store = varstore_new();
		THEN("the store is empty"){
			REQUIRE(varstore_get(store, "") == nullstr);
			REQUIRE(varstore_get(store, "potato") == nullstr);
			REQUIRE(varstore_get(store, "that thing over there") == nullstr);
			REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
			AND_THEN("removing stuff does not work and does nothing"){
				REQUIRE(!IsOk(varstore_remove(store, "potato")));
				REQUIRE(varstore_get(store, "potato") == nullstr);
			}
		}
		THEN("we can add stuff to the store"){
			REQUIRE(IsOk(varstore_add(store, "potato", "big")));
			REQUIRE(IsOk(varstore_add(store, "that thing over there", "An Apache Attack Helicopter")));
			AND_THEN("what we added is there and only"){
				REQUIRE_THAT(varstore_get(store, "potato"), Equals("big"));
				REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
				REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
				AND_WHEN("we override stuff"){
					REQUIRE(IsOk(varstore_add(store, "potato", "smol")));
					THEN("it gets overwritten"){
						REQUIRE_THAT(varstore_get(store, "potato"), Equals("smol"));
						REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
						REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
						AND_WHEN("we remove stuff"){
							REQUIRE(IsOk(varstore_remove(store, "potato")));
							THEN("it's gone"){
								REQUIRE(varstore_get(store, "potato") == nullstr);
								REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
								REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
							}
						}
					}
				}
				AND_WHEN("we add more stuff"){
					auto newthing = GENERATE("crab", "plane", "pixie", "potrolls" "pot", "potat", "potatu", "potatoes", "potato ftw!");
					CAPTURE(newthing);
					REQUIRE(IsOk(varstore_add(store, newthing, "yummy")));
					THEN("it cool"){
						REQUIRE_THAT(varstore_get(store, "potato"), Equals("big"));
						REQUIRE_THAT(varstore_get(store, newthing), Equals("yummy"));
						REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
						REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
						AND_WHEN("we remove stuff"){
							REQUIRE(IsOk(varstore_remove(store, "potato")));
							THEN("it's gone"){
								REQUIRE(varstore_get(store, "potato") == nullstr);
								REQUIRE_THAT(varstore_get(store, newthing), Equals("yummy"));
								REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
								REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
								AND_THEN("removing same thing again is error and does nothing"){
									REQUIRE(!IsOk(varstore_remove(store, "potato")));
									REQUIRE(varstore_get(store, "potato") == nullstr);
									REQUIRE_THAT(varstore_get(store, newthing), Equals("yummy"));
									REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
									REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
								}
								AND_THEN("removing some other thing works ok tho"){
									REQUIRE(IsOk(varstore_remove(store, newthing)));
									REQUIRE(varstore_get(store, "potato") == nullstr);
									REQUIRE(varstore_get(store, newthing) == nullstr);
									REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
									REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
								}
							}
						}
						AND_WHEN("we remove stuff"){
							REQUIRE(IsOk(varstore_remove(store, newthing)));
							THEN("it's gone"){
								REQUIRE_THAT(varstore_get(store, "potato"), Equals("big"));
								REQUIRE(varstore_get(store, newthing) == nullstr);
								REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
								REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
								AND_THEN("removing same thing again is error and does nothing"){
									REQUIRE(!IsOk(varstore_remove(store, newthing)));
									REQUIRE_THAT(varstore_get(store, "potato"), Equals("big"));
									REQUIRE(varstore_get(store, newthing) == nullstr);
									REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
									REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
								}
								AND_THEN("removing some other thing works ok tho"){
									REQUIRE(IsOk(varstore_remove(store, "potato")));
									REQUIRE(varstore_get(store, "potato") == nullstr);
									REQUIRE(varstore_get(store, newthing) == nullstr);
									REQUIRE_THAT(varstore_get(store, "that thing over there"), Equals("An Apache Attack Helicopter"));
									REQUIRE(varstore_get(store, "sherman&p-body") == nullstr);
								}
							}
						}
					}
				}
			}
		}
		varstore_destroy(store);
	}
}

SCENARIO("cloning variables store", "[variables store][parc24]"){
	GIVEN("store with stuff"){
		VarStore store = varstore_new();
		CHECK(IsOk(varstore_add(store, "potato", "bit")));
		CHECK(IsOk(varstore_add(store, "tidbit", "left")));
		CHECK(IsOk(varstore_add(store, "Death", "stranding")));
		WHEN("making a clone"){
			VarStore clone = varstore_clone(store);
			REQUIRE(!!clone);
			THEN("clone keeps the variables"){
				REQUIRE_THAT(varstore_get(clone, "potato"), Equals("bit"));
				REQUIRE_THAT(varstore_get(clone, "tidbit"), Equals("left"));
				REQUIRE_THAT(varstore_get(clone, "Death"), Equals("stranding"));
				AND_THEN("altering one doesn't change the other"){
					REQUIRE(IsOk(varstore_add(store, "potato", "pink")));
					REQUIRE(IsOk(varstore_remove(store, "Death")));
					REQUIRE_THAT(varstore_get(store, "potato"), Equals("pink"));
					REQUIRE_THAT(varstore_get(store, "tidbit"), Equals("left"));
					REQUIRE(varstore_get(store, "Death") == nullstr);
					REQUIRE_THAT(varstore_get(clone, "potato"), Equals("bit"));
					REQUIRE_THAT(varstore_get(clone, "tidbit"), Equals("left"));
					REQUIRE_THAT(varstore_get(clone, "Death"), Equals("stranding"));
					AND_THEN("destroying one keeps the other"){
						varstore_destroy(store);
						store = null;
						REQUIRE_THAT(varstore_get(clone, "potato"), Equals("bit"));
						REQUIRE_THAT(varstore_get(clone, "tidbit"), Equals("left"));
						REQUIRE_THAT(varstore_get(clone, "Death"), Equals("stranding"));
					}
					AND_THEN("destroying the other keeps one"){
						varstore_destroy(clone);
						clone = null;
						REQUIRE_THAT(varstore_get(store, "potato"), Equals("pink"));
						REQUIRE_THAT(varstore_get(store, "tidbit"), Equals("left"));
						REQUIRE(varstore_get(store, "Death") == nullstr);
					}
				}
				AND_THEN("altering the other doesn't change one"){
					REQUIRE(IsOk(varstore_add(clone, "potato", "pink")));
					REQUIRE(IsOk(varstore_remove(clone, "Death")));
					REQUIRE_THAT(varstore_get(clone, "potato"), Equals("pink"));
					REQUIRE_THAT(varstore_get(clone, "tidbit"), Equals("left"));
					REQUIRE(varstore_get(clone, "Death") == nullstr);
					REQUIRE_THAT(varstore_get(store, "potato"), Equals("bit"));
					REQUIRE_THAT(varstore_get(store, "tidbit"), Equals("left"));
					REQUIRE_THAT(varstore_get(store, "Death"), Equals("stranding"));
					AND_THEN("destroying one keeps the other"){
						varstore_destroy(store);
						store = null;
						REQUIRE_THAT(varstore_get(clone, "potato"), Equals("pink"));
						REQUIRE_THAT(varstore_get(clone, "tidbit"), Equals("left"));
						REQUIRE(varstore_get(clone, "Death") == nullstr);
					}
					AND_THEN("destroying the other keeps one"){
						varstore_destroy(clone);
						clone = null;
						REQUIRE_THAT(varstore_get(store, "potato"), Equals("bit"));
						REQUIRE_THAT(varstore_get(store, "tidbit"), Equals("left"));
						REQUIRE_THAT(varstore_get(store, "Death"), Equals("stranding"));
						
					}
				}
			}
			varstore_destroy(clone);
		}
		varstore_destroy(store);
	}
}
