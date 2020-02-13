#ifndef DART_OPERATORS_H
#define DART_OPERATORS_H

/*----- Local Includes -----*/

#include "common.h"
#include "conversion_traits.h"

/*----- Function Definitions -----*/

// I apologize for this whole file.
// I'm not usually a macro kind of guy, but this is one of those rare
// situations where the code would actually be WORSE without the macros.
// It doesn't help that the Dart API surface is so large
namespace dart {

#define DART_DEFINE_EQUALITY_OPERATORS(dart_temp)                                                               \
  template <class T, template <class> class RC, class =                                                         \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, dart_temp<RC>>::value                                                           \
    >                                                                                                           \
  >                                                                                                             \
  bool operator ==(dart_temp<RC> const& lhs, T const& rhs) {                                                    \
    return convert::compare(lhs, rhs);                                                                          \
  }                                                                                                             \
  template <class T, template <class> class RC, class =                                                         \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, dart_temp<RC>>::value                                                           \
    >                                                                                                           \
  >                                                                                                             \
  bool operator ==(T const& lhs, dart_temp<RC> const& rhs) {                                                    \
    return convert::compare(rhs, lhs);                                                                          \
  }                                                                                                             \
  template <class T, template <class> class RC, class =                                                         \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, dart_temp<RC>>::value                                                           \
    >                                                                                                           \
  >                                                                                                             \
  bool operator !=(dart_temp<RC> const& lhs, T const& rhs) {                                                    \
    return !(lhs == rhs);                                                                                       \
  }                                                                                                             \
  template <class T, template <class> class RC, class =                                                         \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, dart_temp<RC>>::value                                                           \
    >                                                                                                           \
  >                                                                                                             \
  bool operator !=(T const& lhs, dart_temp<RC> const& rhs) {                                                    \
    return !(lhs == rhs);                                                                                       \
  }

  DART_DEFINE_EQUALITY_OPERATORS(basic_heap);
  DART_DEFINE_EQUALITY_OPERATORS(basic_buffer);
  DART_DEFINE_EQUALITY_OPERATORS(basic_packet);
#undef DART_DEFINE_EQUALITY_OPERATORS

#define DART_DEFINE_WRAPPER_EQUALITY_OPERATORS(wrapper_temp)                                                    \
  template <class T,                                                                                            \
           template <class> class RC,                                                                           \
           template <template <class> class> class Packet, class =                                              \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, Packet<RC>>::value                                                              \
    >                                                                                                           \
  >                                                                                                             \
  bool operator ==(wrapper_temp<Packet<RC>> const& lhs, T const& rhs) {                                         \
    return convert::compare(lhs.dynamic(), rhs);                                                                \
  }                                                                                                             \
  template <class T,                                                                                            \
           template <class> class RC,                                                                           \
           template <template <class> class> class Packet, class =                                              \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, Packet<RC>>::value                                                              \
    >                                                                                                           \
  >                                                                                                             \
  bool operator ==(T const& lhs, wrapper_temp<Packet<RC>> const& rhs) {                                         \
    return convert::compare(rhs.dynamic(), lhs);                                                                \
  }                                                                                                             \
  template <class T,                                                                                            \
           template <class> class RC,                                                                           \
           template <template <class> class> class Packet, class =                                              \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, Packet<RC>>::value                                                              \
    >                                                                                                           \
  >                                                                                                             \
  bool operator !=(wrapper_temp<Packet<RC>> const& lhs, T const& rhs) {                                         \
    return !(lhs == rhs);                                                                                       \
  }                                                                                                             \
  template <class T,                                                                                            \
           template <class> class RC,                                                                           \
           template <template <class> class> class Packet, class =                                              \
    std::enable_if_t<                                                                                           \
      convert::is_comparable<T, Packet<RC>>::value                                                              \
    >                                                                                                           \
  >                                                                                                             \
  bool operator !=(T const& lhs, wrapper_temp<Packet<RC>> const& rhs) {                                         \
    return !(lhs == rhs);                                                                                       \
  }

  DART_DEFINE_WRAPPER_EQUALITY_OPERATORS(basic_object);
  DART_DEFINE_WRAPPER_EQUALITY_OPERATORS(basic_array);
  DART_DEFINE_WRAPPER_EQUALITY_OPERATORS(basic_string);
  DART_DEFINE_WRAPPER_EQUALITY_OPERATORS(basic_number);
  DART_DEFINE_WRAPPER_EQUALITY_OPERATORS(basic_flag);
  DART_DEFINE_WRAPPER_EQUALITY_OPERATORS(basic_null);
#undef DART_DEFINE_WRAPPER_EQUALITY_OPERATORS

  // Macro defines an std::ostream redirection operator for a wrapper type.
#if DART_HAS_RAPIDJSON
#define DART_DEFINE_WRAPPER_OSTREAM_OPERATOR(wrapper)                                                           \
  template <class Packet>                                                                                       \
  std::ostream& operator <<(std::ostream& out, wrapper<Packet> const& dart) {                                   \
    out << dart.to_json();                                                                                      \
    return out;                                                                                                 \
  }

  DART_DEFINE_WRAPPER_OSTREAM_OPERATOR(basic_object);
  DART_DEFINE_WRAPPER_OSTREAM_OPERATOR(basic_array);
  DART_DEFINE_WRAPPER_OSTREAM_OPERATOR(basic_string);
  DART_DEFINE_WRAPPER_OSTREAM_OPERATOR(basic_number);
  DART_DEFINE_WRAPPER_OSTREAM_OPERATOR(basic_flag);
  DART_DEFINE_WRAPPER_OSTREAM_OPERATOR(basic_null);
#undef DART_DEFINE_WRAPPER_OSTREAM_OPERATOR
#endif

  // Lazy, but effective.
#if DART_HAS_RAPIDJSON
  template <template <template <class> class> class Packet, template <class> class RefCount>
  std::ostream& operator <<(std::ostream& out, Packet<RefCount> const& dart) {
    out << dart.to_json();
    return out;
  }
#endif

}

#endif
