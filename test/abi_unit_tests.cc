/*----- Local Includes -----*/

#include "../include/dart/abi.h"
#include "../include/extern/catch.h"

/*----- Tests -----*/

SCENARIO("objects are regular types", "[abi unit]") {
  GIVEN("a default constructed object") {
    // Get an object.
    auto pkt = dart_obj_init();

    WHEN("the object is queried") {
      THEN("its basic properties make sense") {
        REQUIRE(dart_size(&pkt) == 0);
      }
    }
  }
}
