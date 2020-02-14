#ifndef DART_OPERATORS_H
#define DART_OPERATORS_H

/*----- Local Includes -----*/

#include "common.h"
#include "conversion_traits.h"

/*----- Function Definitions -----*/

namespace dart {

  namespace detail {
    template <class T>
    struct is_dart_api_type : std::false_type {};
    template <template <class> class RC>
    struct is_dart_api_type<dart::basic_heap<RC>> : std::true_type {};
    template <template <class> class RC>
    struct is_dart_api_type<dart::basic_buffer<RC>> : std::true_type {};
    template <template <class> class RC>
    struct is_dart_api_type<dart::basic_packet<RC>> : std::true_type {};
    template <class T>
    struct is_dart_api_type<dart::basic_object<T>> : std::true_type {};
    template <class T>
    struct is_dart_api_type<dart::basic_array<T>> : std::true_type {};
    template <class T>
    struct is_dart_api_type<dart::basic_string<T>> : std::true_type {};
    template <class T>
    struct is_dart_api_type<dart::basic_number<T>> : std::true_type {};
    template <class T>
    struct is_dart_api_type<dart::basic_flag<T>> : std::true_type {};
    template <class T>
    struct is_dart_api_type<dart::basic_null<T>> : std::true_type {};

    template <class Lhs, class Rhs>
    struct dart_comparison_constraints :
      meta::conjunction<
        meta::disjunction<
          detail::is_dart_api_type<std::decay_t<Lhs>>,
          detail::is_dart_api_type<std::decay_t<Rhs>>
        >,
        convert::is_comparable<Lhs>,
        convert::is_comparable<Rhs>
      >
    {};
  }

  template <class Lhs, class Rhs, class =
    std::enable_if_t<
      detail::dart_comparison_constraints<Lhs, Rhs>::value
    >
  >
  bool operator ==(Lhs const& lhs, Rhs const& rhs) {
    return convert::compare(lhs, rhs);
  }

  template <class Lhs, class Rhs, class =
    std::enable_if_t<
      detail::dart_comparison_constraints<Lhs, Rhs>::value
    >
  >
  bool operator !=(Lhs const& lhs, Rhs const& rhs) {
    return !(lhs == rhs);
  }

#if DART_HAS_RAPIDJSON
  template <class Packet, class =
    std::enable_if_t<
      detail::is_dart_api_type<Packet>::value
    >
  >
  std::ostream& operator <<(std::ostream& out, Packet const& dart) {
    out << dart.to_json();
    return out;
  }
#endif

}

#endif
