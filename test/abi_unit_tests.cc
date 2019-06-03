/*----- Local Includes -----*/

#include "../include/dart/abi.h"
#include "../include/extern/catch.h"

/*----- Types -----*/

template <class Func>
struct scope_guard : Func {
  ~scope_guard() noexcept {
    try {
      (*this)();
    } catch (...) {
      // Oops...
    }
  }
};

template <class Func>
auto make_scope_guard(Func&& cb) {
  return scope_guard<std::decay_t<Func>> {cb};
}

/*----- Namespace Inclusions -----*/

using namespace std::string_literals;

/*----- Tests -----*/

SCENARIO("objects are regular types", "[abi unit]") {
  GIVEN("a default constructed object") {
    // Get an object, make sure it's cleaned up.
    auto pkt = dart_obj_init();
    auto guard = make_scope_guard([&] { dart_destroy(&pkt); });

    WHEN("the object is queried") {
      THEN("its basic properties make sense") {
        REQUIRE(dart_size(&pkt) == 0);
        REQUIRE(dart_is_obj(&pkt));
        REQUIRE(pkt.rtti.p_id == DART_PACKET);
        REQUIRE(pkt.rtti.rc_id == DART_RC_SAFE);
        REQUIRE(dart_get_type(&pkt) == DART_OBJECT);
      }
    }

    WHEN("keys are inserted") {
      // Insert some values into our object.
      dart_obj_insert_str(&pkt, "hello", "world");
      dart_obj_insert_int(&pkt, "int", 5);
      dart_obj_insert_dcm(&pkt, "pi", 3.14159);
      dart_obj_insert_bool(&pkt, "bool", true);

      THEN("the keys are accessible") {
        // Grab each key for validation, make sure it's cleaned up.
        REQUIRE(dart_size(&pkt) == 4U);
        auto key_one = dart_obj_get(&pkt, "hello");
        auto key_two = dart_obj_get(&pkt, "int");
        auto key_three = dart_obj_get(&pkt, "pi");
        auto key_four = dart_obj_get(&pkt, "bool");
        auto guard = make_scope_guard([&] {
          dart_destroy(&key_one);
          dart_destroy(&key_two);
          dart_destroy(&key_three);
          dart_destroy(&key_four);
        });
        REQUIRE(dart_is_str(&key_one));
        REQUIRE(dart_str_get(&key_one) == "world"s);
        REQUIRE(dart_is_int(&key_two));
        REQUIRE(dart_int_get(&key_two) == 5);
        REQUIRE(dart_is_dcm(&key_three));
        REQUIRE(dart_dcm_get(&key_three) == 3.14159);
        REQUIRE(dart_is_bool(&key_four));
        REQUIRE(dart_bool_get(&key_four) == true);
      }
    }

    WHEN("aggregates are inserted") {
      auto nested = dart_obj_init();
      auto guard = make_scope_guard([&] { dart_destroy(&nested); });
      dart_obj_insert_str(&nested, "a nested", "string");
      dart_obj_insert_dart(&pkt, "nested", &nested);
      THEN("it's recursively queryable") {
        auto nested_copy = dart_obj_get(&pkt, "nested");
        auto nested_str = dart_obj_get(&nested_copy, "a nested");
        auto guard = make_scope_guard([&] {
          dart_destroy(&nested_str);
          dart_destroy(&nested_copy);
        });
        REQUIRE(dart_is_str(&nested_str));
        REQUIRE(dart_str_get(&nested_str) == "string"s);
        REQUIRE(dart_size(&pkt) == 1U);
        REQUIRE(dart_is_obj(&nested_copy));
        REQUIRE(dart_size(&nested_copy) == 1U);
        REQUIRE(dart_equal(&nested_copy, &nested));
      }
    }
  }
}

SCENARIO("arrays can be iterated over", "[abi unit]") {
  GIVEN("an array with contents") {
    auto* dyn = "dynamic";
    auto arr = dart_arr_init_va("idbsS", 1, 3.14159, 0, "fixed", dyn, strlen(dyn));
    auto guard = make_scope_guard([&] { dart_destroy(&arr); });

    WHEN("we create an iterator") {
      // Initialize an iterator for our array.
      dart_iterator_t it;
      dart_iterator_init_err(&it, &arr);

      THEN("it visits all values") {
        REQUIRE(!dart_iterator_done(&it));
        auto one = dart_iterator_get(&it);
        dart_iterator_next(&it);
        auto two = dart_iterator_get(&it);
        dart_iterator_next(&it);
        auto three = dart_iterator_get(&it);
        dart_iterator_next(&it);
        auto four = dart_iterator_get(&it);
        dart_iterator_next(&it);
        auto five = dart_iterator_get(&it);
        dart_iterator_next(&it);
        auto guard = make_scope_guard([&] {
          dart_destroy(&one);
          dart_destroy(&two);
          dart_destroy(&three);
          dart_destroy(&four);
          dart_destroy(&five);
        });
        REQUIRE(dart_iterator_done(&it));
        dart_iterator_destroy(&it);

        REQUIRE(dart_is_int(&one));
        REQUIRE(dart_int_get(&one) == 1);
        REQUIRE(dart_is_dcm(&two));
        REQUIRE(dart_dcm_get(&two) == 3.14159);
        REQUIRE(dart_is_bool(&three));
        REQUIRE(dart_bool_get(&three) == false);
        REQUIRE(dart_is_str(&four));
        REQUIRE(dart_str_get(&four) == "fixed"s);
        REQUIRE(dart_str_get(&five) == "dynamic"s);
      }
    }

    WHEN("we use automatic iteration") {
      int idx = 0;
      dart_packet_t val;
      THEN("it visits all values in order") {
        dart_for_each(&arr, &val) {
          // Get the value manually.
          auto verify = dart_arr_get(&arr, idx++);
          auto guard = make_scope_guard([&] { dart_destroy(&verify); });

          // Check it
          REQUIRE(!dart_is_null(&val));
          REQUIRE(!dart_is_null(&verify));
          REQUIRE(dart_get_type(&val) == dart_get_type(&verify));
          REQUIRE(dart_equal(&val, &verify));
        }
      }
    }
  }
}
