#ifndef DART_PRIMITIVE_H
#define DART_PRIMITIVE_H

/*----- Local Includes -----*/

#include "common.h"

/*----- Function Implementations -----*/

namespace dart {

  template <class Number>
  template <class Num, class EnableIf>
  basic_number<Number>::basic_number(Num&& val) {
    if (!val.is_numeric()) {
      throw type_error("dart::packet::number can only be constructed from a numeric value");
    }
    this->val = std::forward<Num>(val);
  }

  template <class Boolean>
  template <class Bool, class EnableIf>
  basic_flag<Boolean>::basic_flag(Bool&& val) {
    if (!val.is_boolean()) {
      throw type_error("dart::packet::flag can only be constructed from a boolean value");
    }
    this->val = std::forward<Bool>(val);
  }

  template <class Number>
  template <class Arg, class EnableIf>
  basic_number<Number>& basic_number<Number>::operator +=(Arg val) noexcept {
    this->val = numeric() + val;
    return *this;
  }

  template <class Number>
  template <class Arg, class EnableIf>
  basic_number<Number>& basic_number<Number>::operator -=(Arg val) noexcept {
    this->val = numeric() - val;
    return *this;
  }

  template <class Number>
  template <class Arg, class EnableIf>
  basic_number<Number>& basic_number<Number>::operator *=(Arg val) noexcept {
    this->val = numeric() * val;
    return *this;
  }

  template <class Number>
  template <class Arg, class EnableIf>
  basic_number<Number>& basic_number<Number>::operator /=(Arg val) noexcept {
    this->val = numeric() / val;
    return *this;
  }

  template <class Number>
  template <class Num, class EnableIf>
  basic_number<Number>& basic_number<Number>::operator ++() noexcept {
    val = numeric() + 1;
    return *this;
  }

  template <class Number>
  template <class Num, class EnableIf>
  basic_number<Number>& basic_number<Number>::operator --() noexcept {
    val = numeric() - 1;
    return *this;
  }

  template <class Number>
  template <class Num, class EnableIf>
  basic_number<Number> basic_number<Number>::operator ++(int) noexcept {
    auto copy = *this;
    val = numeric() + 1;
    return copy;
  }

  template <class Number>
  template <class Num, class EnableIf>
  basic_number<Number> basic_number<Number>::operator --(int) noexcept {
    auto copy = *this;
    val = numeric() - 1;
    return copy;
  }

  template <class Number>
  int64_t basic_number<Number>::integer() const {
    return val.integer();
  }

  template <class Number>
  double basic_number<Number>::decimal() const {
    return val.decimal();
  }

  template <class Number>
  double basic_number<Number>::numeric() const noexcept {
    return val.numeric();
  }

  template <class Boolean>
  bool basic_flag<Boolean>::boolean() const noexcept {
    return val.boolean();
  }

  namespace detail {

    template <class T>
    template <bool silent>
    bool primitive<T>::is_valid(size_t bytes) const noexcept(silent) {
      // With a primitive value, there's only the header, so check to ensure
      // it's within bounds
      if (bytes < header_len) {
        if (silent) return false;
        else throw validation_error("Serialized primitive value is truncated");
      }
      return true;
    }

    template <class T>
    size_t primitive<T>::get_sizeof() const noexcept {
      return sizeof(T);
    }

    template <class T>
    T primitive<T>::get_data() const noexcept {
      return data;
    }

    template <class T>
    size_t primitive<T>::static_sizeof() noexcept {
      return sizeof(primitive<T>);
    }

  }

}

#endif
