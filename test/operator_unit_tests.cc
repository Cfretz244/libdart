/*----- System Includes -----*/

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

/*----- Local Includes -----*/

#include "dart_tests.h"

/*----- Function Implementations -----*/

SCENARIO("mutable dart types can be assigned to from many types", "[operator unit]") {
  GIVEN("a default constructed dart type") {
    dart::mutable_api_test([] (auto tag, auto idx) {
      using pkt = typename decltype(tag)::type;
      using map = std::map<std::string, std::string>;

      // Get a default constructed instance.
      pkt val;

      DYNAMIC_WHEN("the value is assigned from a map", idx) {
        val = map {{"hello", "world"}, {"yes", "no"}};
        DYNAMIC_THEN("it takes on the value we expect", idx) {
        }
      }

      // Get an object.
      auto obj = pkt::make_object();

      // Check to make sure the type agrees.
      REQUIRE(obj.is_object());
      REQUIRE(obj.get_type() == dart::packet::type::object);

      // Check to make sure the object is empty.
      REQUIRE(obj.size() == 0ULL);

      DYNAMIC_WHEN("the object is finalized", idx) {
        auto immutable = obj.finalize();
        DYNAMIC_THEN("basic properties remain the same", idx) {
          // Check to make sure the type agrees.
          REQUIRE(immutable.is_object());
          REQUIRE(immutable.get_type() == dart::packet::type::object);

          // Check to make sure the object is empty.
          REQUIRE(immutable.size() == 0ULL);
        }
      }
    });
  }
}
