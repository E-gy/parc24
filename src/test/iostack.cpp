#include <catch2ext.hpp>
using Catch::Matchers::Equals;

extern "C" {
#include <parc24/iosstack.h>
#include <util/null.h>
#include <cppo.h>
#include <cppo/parallels.h>
}

SCENARIO("io stack does the stacking", "[io stack]"){
	GIVEN("empty IO stack"){
		IOsStack ios = iosstack_new();
		auto p1r = pipe_new(), p2r = pipe_new();
		REQUIRE(IsOk_T(p1r));
		REQUIRE(IsOk_T(p2r));
		auto p1 = p1r.r.ok, p2 = p2r.r.ok;
		THEN("opening is OK"){
			REQUIRE(IsOk(iostack_io_open(ios, 4, p1.read)));
			REQUIRE(IsOk(iostack_io_open(ios, 6, p1.write)));
			REQUIRE(iosstack_raw_get(ios, 4) == p1.read);
			REQUIRE(iosstack_raw_get(ios, 6) == p1.write);
			AND_THEN("we can just overwrite the streams"){
				REQUIRE(IsOk(iostack_io_open(ios, 4, p2.read)));
				REQUIRE(IsOk(iostack_io_open(ios, 6, p2.write)));
				REQUIRE(iosstack_raw_get(ios, 4) == p2.read);
				REQUIRE(iosstack_raw_get(ios, 6) == p2.write);
			}
			WHEN("state is pushed"){
				REQUIRE(IsOk(iosstack_push(ios)));
				THEN("we can overwrite the streams"){
					REQUIRE(IsOk(iostack_io_open(ios, 4, p2.read)));
					REQUIRE(IsOk(iostack_io_open(ios, 6, p2.write)));
					REQUIRE(iosstack_raw_get(ios, 4) == p2.read);
					REQUIRE(iosstack_raw_get(ios, 6) == p2.write);
					AND_WHEN("we pop the state"){
						REQUIRE(IsOk(iosstack_pop(ios)));
						THEN("stack is back to where it was"){
							REQUIRE(iosstack_raw_get(ios, 4) == p1.read);
							REQUIRE(iosstack_raw_get(ios, 6) == p1.write);
							AND_THEN("we can do it again, for fun idk"){
								auto p3r = pipe_new();
								REQUIRE(IsOk_T(p3r));
								auto p3 = p3r.r.ok;
								REQUIRE(IsOk(iosstack_push(ios)));
								REQUIRE(IsOk(iostack_io_open(ios, 4, p3.read)));
								REQUIRE(IsOk(iostack_io_open(ios, 6, p3.write)));
								REQUIRE(iosstack_raw_get(ios, 4) == p3.read);
								REQUIRE(iosstack_raw_get(ios, 6) == p3.write);
								REQUIRE(IsOk(iosstack_pop(ios)));
								REQUIRE(iosstack_raw_get(ios, 4) == p1.read);
								REQUIRE(iosstack_raw_get(ios, 6) == p1.write);
							}
						}
					}
					AND_WHEN("push again"){
						auto p3r = pipe_new();
						REQUIRE(IsOk_T(p3r));
						auto p3 = p3r.r.ok;
						REQUIRE(IsOk(iosstack_push(ios)));
						THEN("stack go brrrr"){
							REQUIRE(IsOk(iostack_io_open(ios, 4, p3.read)));
							REQUIRE(IsOk(iostack_io_open(ios, 6, p3.write)));
							REQUIRE(iosstack_raw_get(ios, 4) == p3.read);
							REQUIRE(iosstack_raw_get(ios, 6) == p3.write);
							REQUIRE(IsOk(iosstack_pop(ios)));
							REQUIRE(iosstack_raw_get(ios, 4) == p2.read);
							REQUIRE(iosstack_raw_get(ios, 6) == p2.write);
							REQUIRE(IsOk(iosstack_pop(ios)));
							REQUIRE(iosstack_raw_get(ios, 4) == p1.read);
							REQUIRE(iosstack_raw_get(ios, 6) == p1.write);
						}
					}
				}
			}
		}
		iosstack_destroy(ios);
	}
}
