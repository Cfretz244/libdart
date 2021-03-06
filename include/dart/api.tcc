#ifndef DART_API_H
#define DART_API_H

/*----- System Includes -----*/

#include <assert.h>

/*----- Local Includes -----*/

#include "common.h"

/*----- Function Implementations -----*/

namespace dart {

  template <class Null>
  basic_null<Null>::basic_null(Null const& null) {
    if (!null.is_null()) throw type_error("dart::packet::null can only be constructed as a null");
  }

  template <class Object>
  template <class Key, class Value, class Comp, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::map<Key, Value, Comp, Alloc> const& map) & {
    val = convert::cast<value_type>(map);
    return *this;
  }

  template <class Object>
  template <class Key, class Value, class Comp, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::map<Key, Value, Comp, Alloc>&& map) & {
    val = convert::cast<value_type>(std::move(map));
    return *this;
  }

  template <class Object>
  template <class Key, class Value, class Comp, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::multimap<Key, Value, Comp, Alloc> const& map) & {
    val = convert::cast<value_type>(map);
    return *this;
  }

  template <class Object>
  template <class Key, class Value, class Comp, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::multimap<Key, Value, Comp, Alloc>&& map) & {
    val = convert::cast<value_type>(std::move(map));
    return *this;
  }

  template <class Object>
  template <class Key, class Value, class Hash, class Equal, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::unordered_map<Key, Value, Hash, Equal, Alloc> const& map) & {
    val = convert::cast<value_type>(map);
    return *this;
  }

  template <class Object>
  template <class Key, class Value, class Hash, class Equal, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::unordered_map<Key, Value, Hash, Equal, Alloc>&& map) & {
    val = convert::cast<value_type>(std::move(map));
    return *this;
  }

  template <class Object>
  template <class Key, class Value, class Hash, class Equal, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::unordered_multimap<Key, Value, Hash, Equal, Alloc> const& map) & {
    val = convert::cast<value_type>(map);
    return *this;
  }

  template <class Object>
  template <class Key, class Value, class Hash, class Equal, class Alloc, class EnableIf>
  basic_object<Object>& basic_object<Object>::operator =(std::unordered_multimap<Key, Value, Hash, Equal, Alloc>&& map) & {
    val = convert::cast<value_type>(std::move(map));
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::vector<T, Alloc> const& vec) & {
    val = convert::cast<value_type>(vec);
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::vector<T, Alloc>&& vec) & {
    val = convert::cast<value_type>(std::move(vec));
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::deque<T, Alloc> const& deq) & {
    val = convert::cast<value_type>(deq);
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::deque<T, Alloc>&& deq) & {
    val = convert::cast<value_type>(std::move(deq));
    return *this;
  }

  template <class Array>
  template <class T, size_t len, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::array<T, len> const& arr) & {
    val = convert::cast<value_type>(arr);
    return *this;
  }

  template <class Array>
  template <class T, size_t len, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::array<T, len>&& arr) & {
    val = convert::cast<value_type>(std::move(arr));
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::list<T, Alloc> const& lst) & {
    val = convert::cast<value_type>(lst);
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::list<T, Alloc>&& lst) & {
    val = convert::cast<value_type>(std::move(lst));
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::forward_list<T, Alloc> const& flst) & {
    val = convert::cast<value_type>(flst);
    return *this;
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::forward_list<T, Alloc>&& flst) & {
    val = convert::cast<value_type>(std::move(flst));
    return *this;
  }

  template <class Array>
  template <class Key, class Compare, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::set<Key, Compare, Alloc> const& set) & {
    val = convert::cast<value_type>(set);
    return *this;
  }

  template <class Array>
  template <class Key, class Compare, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::set<Key, Compare, Alloc>&& set) & {
    val = convert::cast<value_type>(std::move(set));
    return *this;
  }

  template <class Array>
  template <class Key, class Hash, class KeyEq, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::unordered_set<Key, Hash, KeyEq, Alloc> const& set) & {
    val = convert::cast<value_type>(set);
    return *this;
  }

  template <class Array>
  template <class Key, class Hash, class KeyEq, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::unordered_set<Key, Hash, KeyEq, Alloc>&& set) & {
    val = convert::cast<value_type>(std::move(set));
    return *this;
  }

  template <class Array>
  template <class Key, class Compare, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::multiset<Key, Compare, Alloc> const& set) & {
    val = convert::cast<value_type>(set);
    return *this;
  }

  template <class Array>
  template <class Key, class Compare, class Alloc, class EnableIf>
  basic_array<Array>& basic_array<Array>::operator =(std::multiset<Key, Compare, Alloc>&& set) & {
    val = convert::cast<value_type>(std::move(set));
    return *this;
  }

  template <class String>
  template <class Str, class EnableIf>
  basic_string<String>& basic_string<String>::operator =(shim::string_view str) & {
    val = convert::cast<value_type>(str);
    return *this;
  }

  template <class Number>
  template <class Num, class EnableIf>
  basic_number<Number>& basic_number<Number>::operator =(Num val) & {
    this->val = convert::cast<value_type>(val);
    return *this;
  }

  template <class Boolean>
  template <class Bool, class EnableIf>
  basic_flag<Boolean>& basic_flag<Boolean>::operator =(bool val) & {
    this->val = convert::cast<value_type>(val);
    return *this;
  }

  template <class Null>
  basic_null<Null>& basic_null<Null>::operator =(std::nullptr_t) & {
    return *this;
  }

  template <class String>
  shim::string_view basic_string<String>::operator *() const noexcept {
    return strv();
  }

  template <class Number>
  double basic_number<Number>::operator *() const noexcept {
    return numeric();
  }

  template <class Boolean>
  bool basic_flag<Boolean>::operator *() const noexcept {
    return boolean();
  }

  template <class Object>
  basic_object<Object>::operator view() const& noexcept {
    return view {typename value_type::view {val}};
  }

  template <class Array>
  basic_array<Array>::operator view() const& noexcept {
    return view {typename value_type::view {val}};
  }

  template <class String>
  basic_string<String>::operator view() const& noexcept {
    return view {typename value_type::view {val}};
  }

  template <class Number>
  basic_number<Number>::operator view() const& noexcept {
    return view {typename value_type::view {val}};
  }

  template <class Boolean>
  basic_flag<Boolean>::operator view() const& noexcept {
    return view {typename value_type::view {val}};
  }

  template <class Null>
  basic_null<Null>::operator view() const& noexcept {
    return view {nullptr};
  }

  template <class Object>
  basic_object<Object>::operator value_type() const& noexcept {
    return val;
  }

  template <class Object>
  basic_object<Object>::operator value_type() && noexcept {
    return std::move(val);
  }

  template <class Array>
  basic_array<Array>::operator value_type() const& noexcept {
    return val;
  }

  template <class Array>
  basic_array<Array>::operator value_type() && noexcept {
    return std::move(val);
  }

  template <class String>
  basic_string<String>::operator value_type() const& noexcept {
    return val;
  }

  template <class String>
  basic_string<String>::operator value_type() && noexcept {
    return std::move(val);
  }

  template <class Number>
  basic_number<Number>::operator value_type() const& noexcept {
    return val;
  }

  template <class Number>
  basic_number<Number>::operator value_type() && noexcept {
    return std::move(val);
  }

  template <class Boolean>
  basic_flag<Boolean>::operator value_type() const& noexcept {
    return val;
  }

  template <class Boolean>
  basic_flag<Boolean>::operator value_type() && noexcept {
    return std::move(val);
  }

  template <class Null>
  basic_null<Null>::operator value_type() const noexcept {
    return value_type::make_null();
  }

  template <class Object>
  basic_object<Object>::operator bool() const noexcept {
    return !is_null();
  }

  template <class Array>
  basic_array<Array>::operator bool() const noexcept {
    return !is_null();
  }

  template <class String>
  basic_string<String>::operator bool() const noexcept {
    return !is_null();
  }

  template <class Number>
  basic_number<Number>::operator bool() const noexcept {
    return !is_null();
  }

  template <class Boolean>
  basic_flag<Boolean>::operator bool() const noexcept {
    return !is_null() && boolean();
  }

  template <class Null>
  constexpr basic_null<Null>::operator bool() const noexcept {
    return false;
  }

  template <class Object>
  template <class Key, class Value, class Comp, class Alloc, class EnableIf>
  basic_object<Object>::operator std::map<Key, Value, Comp, Alloc>() const {
    return convert::cast<std::map<Key, Value, Comp, Alloc>>(val);
  }

  template <class Object>
  template <class Key, class Value, class Comp, class Alloc, class EnableIf>
  basic_object<Object>::operator std::multimap<Key, Value, Comp, Alloc>() const {
    return convert::cast<std::multimap<Key, Value, Comp, Alloc>>(val);
  }

  template <class Object>
  template <class Key, class Value, class Hash, class Equal, class Alloc, class EnableIf>
  basic_object<Object>::operator std::unordered_map<Key, Value, Hash, Equal, Alloc>() const {
    return convert::cast<std::unordered_map<Key, Value, Hash, Equal, Alloc>>(val);
  }

  template <class Object>
  template <class Key, class Value, class Hash, class Equal, class Alloc, class EnableIf>
  basic_object<Object>::operator std::unordered_multimap<Key, Value, Hash, Equal, Alloc>() const {
    return convert::cast<std::unordered_multimap<Key, Value, Hash, Equal, Alloc>>(val);
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>::operator std::vector<T, Alloc>() const {
    return convert::cast<std::vector<T, Alloc>>(val);
  }

  template <class Array>
  template <class T, size_t len, class EnableIf>
  basic_array<Array>::operator std::array<T, len>() const {
    return convert::cast<std::array<T, len>>(val);
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>::operator std::deque<T, Alloc>() const {
    return convert::cast<std::deque<T, Alloc>>(val);
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>::operator std::list<T, Alloc>() const {
    return convert::cast<std::list<T, Alloc>>(val);
  }

  template <class Array>
  template <class T, class Alloc, class EnableIf>
  basic_array<Array>::operator std::forward_list<T, Alloc>() const {
    return convert::cast<std::forward_list<T, Alloc>>(val);
  }

  template <class Array>
  template <class Key, class Compare, class Alloc, class EnableIf>
  basic_array<Array>::operator std::set<Key, Compare, Alloc>() const {
    return convert::cast<std::set<Key, Compare, Alloc>>(val);
  }

  template <class Array>
  template <class Key, class Hash, class KeyEq, class Alloc, class EnableIf>
  basic_array<Array>::operator std::unordered_set<Key, Hash, KeyEq, Alloc>() const {
    return convert::cast<std::unordered_set<Key, Hash, KeyEq, Alloc>>(val);
  }

  template <class Array>
  template <class Key, class Compare, class Alloc, class EnableIf>
  basic_array<Array>::operator std::multiset<Key, Compare, Alloc>() const {
    return convert::cast<std::multiset<Key, Compare, Alloc>>(val);
  }

  template <class Array>
  template <class Key, class Hash, class KeyEq, class Alloc, class EnableIf>
  basic_array<Array>::operator std::unordered_multiset<Key, Hash, KeyEq, Alloc>() const {
    return convert::cast<std::unordered_multiset<Key, Hash, KeyEq, Alloc>>(val);
  }

  template <class String>
  basic_string<String>::operator std::string() const {
    return std::string {val};
  }

  template <class String>
  basic_string<String>::operator shim::string_view() const noexcept {
    return shim::string_view {val};
  }

  template <class Number>
  template <class T,
    std::enable_if_t<
      std::is_integral<T>::value
      ||
      std::is_floating_point<T>::value
    >*
  >
  basic_number<Number>::operator T() const noexcept {
    if (is_integer()) return static_cast<T>(integer());
    else return static_cast<T>(decimal());
  }

  template <class Object>
  decltype(auto) basic_object<Object>::definalize() & {
    return val.definalize();
  }

  template <class Object>
  decltype(auto) basic_object<Object>::definalize() && {
    return std::move(val).definalize();
  }

  template <class Object>
  decltype(auto) basic_object<Object>::lift() & {
    return val.lift();
  }

  template <class Object>
  decltype(auto) basic_object<Object>::lift() && {
    return std::move(val).lift();
  }

  template <class Object>
  decltype(auto) basic_object<Object>::finalize() & {
    return val.finalize();
  }

  template <class Object>
  decltype(auto) basic_object<Object>::finalize() && {
    return std::move(val).finalize();
  }

  template <class Object>
  decltype(auto) basic_object<Object>::lower() & {
    return val.lower();
  }

  template <class Object>
  decltype(auto) basic_object<Object>::lower() && {
    return std::move(val).lower();
  }

  template <class Object>
  auto basic_object<Object>::values() const -> std::vector<value_type> {
    return val.values();
  }

  template <class Array>
  auto basic_array<Array>::values() const -> std::vector<value_type> {
    return val.values();
  }

  template <class Object>
  template <class Obj, class EnableIf>
  auto basic_object<Object>::get_bytes() const {
    return val.get_bytes();
  }

  template <class Object>
  template <class RC, class EnableIf>
  auto basic_object<Object>::share_bytes(RC& bytes) const {
    return val.share_bytes(bytes);
  }

  template <class Object>
  template <class Obj, class EnableIf>
  auto basic_object<Object>::dup_bytes() const {
    size_type dummy;
    return val.dup_bytes(dummy);
  }

  template <class Object>
  template <class Obj, class EnableIf>
  auto basic_object<Object>::dup_bytes(size_type& len) const {
    return val.dup_bytes(len);
  }

  template <class Object>
  auto basic_object<Object>::size() const noexcept -> size_type {
    return val.size();
  }

  template <class Array>
  auto basic_array<Array>::size() const noexcept -> size_type {
    return val.size();
  }

  template <class String>
  auto basic_string<String>::size() const noexcept -> size_type {
    return val.size();
  }

  template <class Object>
  bool basic_object<Object>::empty() const noexcept {
    return val.empty();
  }

  template <class Array>
  bool basic_array<Array>::empty() const noexcept {
    return val.empty();
  }

  template <class String>
  bool basic_string<String>::empty() const noexcept {
    return val.empty();
  }

  template <class Object>
  auto basic_object<Object>::dynamic() const& noexcept -> value_type const& {
    return val;
  }

  template <class Object>
  auto basic_object<Object>::dynamic() && noexcept -> value_type&& {
    return std::move(val);
  }

  template <class Array>
  auto basic_array<Array>::dynamic() const& noexcept -> value_type const& {
    return val;
  }

  template <class Array>
  auto basic_array<Array>::dynamic() && noexcept -> value_type&& {
    return std::move(val);
  }

  template <class String>
  auto basic_string<String>::dynamic() const& noexcept -> value_type const& {
    return val;
  }

  template <class String>
  auto basic_string<String>::dynamic() && noexcept -> value_type&& {
    return std::move(val);
  }

  template <class Number>
  auto basic_number<Number>::dynamic() const& noexcept -> value_type const& {
    return val;
  }

  template <class Number>
  auto basic_number<Number>::dynamic() && noexcept -> value_type&& {
    return std::move(val);
  }

  template <class Boolean>
  auto basic_flag<Boolean>::dynamic() const& noexcept -> value_type const& {
    return val;
  }

  template <class Boolean>
  auto basic_flag<Boolean>::dynamic() && noexcept -> value_type&& {
    return std::move(val);
  }

  template <class Null>
  auto basic_null<Null>::dynamic() const& noexcept -> value_type const& {
    return val;
  }

  template <class Null>
  auto basic_null<Null>::dynamic() && noexcept -> value_type&& {
    return std::move(val);
  }

  template <class Object>
  bool basic_object<Object>::is_object() const noexcept {
    return val.is_object();
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_object() const noexcept {
    return false;
  }

  template <class String>
  constexpr bool basic_string<String>::is_object() const noexcept {
    return false;
  }

  template <class Number>
  constexpr bool basic_number<Number>::is_object() const noexcept {
    return false;
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_object() const noexcept {
    return false;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_object() const noexcept {
    return false;
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_array() const noexcept {
    return false;
  }

  template <class Array>
  bool basic_array<Array>::is_array() const noexcept {
    return val.is_array();
  }

  template <class String>
  constexpr bool basic_string<String>::is_array() const noexcept {
    return false;
  }

  template <class Number>
  constexpr bool basic_number<Number>::is_array() const noexcept {
    return false;
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_array() const noexcept {
    return false;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_array() const noexcept {
    return false;
  }

  template <class Object>
  bool basic_object<Object>::is_aggregate() const noexcept {
    return is_object();
  }

  template <class Array>
  bool basic_array<Array>::is_aggregate() const noexcept {
    return is_array();
  }

  template <class String>
  constexpr bool basic_string<String>::is_aggregate() const noexcept {
    return false;
  }

  template <class Number>
  constexpr bool basic_number<Number>::is_aggregate() const noexcept {
    return false;
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_aggregate() const noexcept {
    return false;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_aggregate() const noexcept {
    return false;
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_str() const noexcept {
    return false;
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_str() const noexcept {
    return false;
  }

  template <class String>
  bool basic_string<String>::is_str() const noexcept {
    return val.is_str();
  }

  template <class Number>
  constexpr bool basic_number<Number>::is_str() const noexcept {
    return false;
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_str() const noexcept {
    return false;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_str() const noexcept {
    return false;
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_integer() const noexcept {
    return false;
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_integer() const noexcept {
    return false;
  }

  template <class String>
  constexpr bool basic_string<String>::is_integer() const noexcept {
    return false;
  }

  template <class Number>
  bool basic_number<Number>::is_integer() const noexcept {
    return val.is_integer();
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_integer() const noexcept {
    return false;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_integer() const noexcept {
    return false;
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_decimal() const noexcept {
    return false;
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_decimal() const noexcept {
    return false;
  }

  template <class String>
  constexpr bool basic_string<String>::is_decimal() const noexcept {
    return false;
  }

  template <class Number>
  bool basic_number<Number>::is_decimal() const noexcept {
    return val.is_decimal();
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_decimal() const noexcept {
    return false;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_decimal() const noexcept {
    return false;
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_numeric() const noexcept {
    return false;
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_numeric() const noexcept {
    return false;
  }

  template <class String>
  constexpr bool basic_string<String>::is_numeric() const noexcept {
    return false;
  }

  template <class Number>
  bool basic_number<Number>::is_numeric() const noexcept {
    return !is_null();
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_numeric() const noexcept {
    return false;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_numeric() const noexcept {
    return false;
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_boolean() const noexcept {
    return false;
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_boolean() const noexcept {
    return false;
  }

  template <class String>
  constexpr bool basic_string<String>::is_boolean() const noexcept {
    return false;
  }

  template <class Number>
  constexpr bool basic_number<Number>::is_boolean() const noexcept {
    return false;
  }

  template <class Boolean>
  bool basic_flag<Boolean>::is_boolean() const noexcept {
    return val.is_boolean();
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_boolean() const noexcept {
    return false;
  }

  template <class Object>
  bool basic_object<Object>::is_null() const noexcept {
    return val.is_null();
  }

  template <class Array>
  bool basic_array<Array>::is_null() const noexcept {
    return val.is_null();
  }

  template <class String>
  bool basic_string<String>::is_null() const noexcept {
    return val.is_null();
  }

  template <class Number>
  bool basic_number<Number>::is_null() const noexcept {
    return val.is_null();
  }

  template <class Boolean>
  bool basic_flag<Boolean>::is_null() const noexcept {
    return val.is_null();
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_null() const noexcept {
    return true;
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_primitive() const noexcept {
    return false;
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_primitive() const noexcept {
    return false;
  }

  template <class String>
  constexpr bool basic_string<String>::is_primitive() const noexcept {
    return true;
  }

  template <class Number>
  constexpr bool basic_number<Number>::is_primitive() const noexcept {
    return true;
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_primitive() const noexcept {
    return true;
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_primitive() const noexcept {
    return true;
  }

  template <class Object>
  auto basic_object<Object>::get_type() const noexcept -> type {
    return val.get_type();
  }

  template <class Array>
  auto basic_array<Array>::get_type() const noexcept -> type {
    return val.get_type();
  }

  template <class String>
  auto basic_string<String>::get_type() const noexcept -> type {
    return val.get_type();
  }

  template <class Number>
  auto basic_number<Number>::get_type() const noexcept -> type {
    return val.get_type();
  }

  template <class Boolean>
  auto basic_flag<Boolean>::get_type() const noexcept -> type {
    return val.get_type();
  }

  template <class Null>
  constexpr auto basic_null<Null>::get_type() const noexcept -> type {
    return type::null;
  }

  template <class Object>
  bool basic_object<Object>::is_finalized() const noexcept {
    return val.is_finalized();
  }

  template <class Array>
  bool basic_array<Array>::is_finalized() const noexcept {
    return val.is_finalized();
  }

  template <class String>
  bool basic_string<String>::is_finalized() const noexcept {
    return val.is_finalized();
  }

  template <class Number>
  bool basic_number<Number>::is_finalized() const noexcept {
    return val.is_finalized();
  }

  template <class Boolean>
  bool basic_flag<Boolean>::is_finalized() const noexcept {
    return val.is_finalized();
  }

  template <class Null>
  bool basic_null<Null>::is_finalized() const noexcept {
    return value_type::make_null().is_finalized();
  }

  template <class Object>
  auto basic_object<Object>::refcount() const noexcept -> size_type {
    return val.refcount();
  }

  template <class Array>
  auto basic_array<Array>::refcount() const noexcept -> size_type {
    return val.refcount();
  }

  template <class String>
  auto basic_string<String>::refcount() const noexcept -> size_type {
    return val.refcount();
  }

  template <class Number>
  auto basic_number<Number>::refcount() const noexcept -> size_type {
    return val.refcount();
  }

  template <class Boolean>
  auto basic_flag<Boolean>::refcount() const noexcept -> size_type {
    return val.refcount();
  }

  template <class Null>
  auto basic_null<Null>::refcount() const noexcept -> size_type {
    return value_type::make_null().refcount();
  }

  template <class Object>
  auto basic_object<Object>::begin() const -> iterator {
    return val.begin();
  }

  template <class Array>
  auto basic_array<Array>::begin() const -> iterator {
    return val.begin();
  }

  template <class Object>
  auto basic_object<Object>::cbegin() const -> iterator {
    return val.cbegin();
  }

  template <class Array>
  auto basic_array<Array>::cbegin() const -> iterator {
    return val.cbegin();
  }

  template <class Object>
  auto basic_object<Object>::end() const -> iterator {
    return val.end();
  }

  template <class Array>
  auto basic_array<Array>::end() const -> iterator {
    return val.end();
  }

  template <class Object>
  auto basic_object<Object>::cend() const -> iterator {
    return val.cend();
  }

  template <class Array>
  auto basic_array<Array>::cend() const -> iterator {
    return val.cend();
  }

  template <class Object>
  auto basic_object<Object>::rbegin() const -> reverse_iterator {
    return val.rbegin();
  }

  template <class Array>
  auto basic_array<Array>::rbegin() const -> reverse_iterator {
    return val.rbegin();
  }

  template <class Object>
  auto basic_object<Object>::rend() const -> reverse_iterator {
    return val.rend();
  }

  template <class Array>
  auto basic_array<Array>::rend() const -> reverse_iterator {
    return val.rend();
  }

  template <class Object>
  auto basic_object<Object>::key_begin() const -> iterator {
    return val.key_begin();
  }

  template <class Object>
  auto basic_object<Object>::rkey_begin() const -> reverse_iterator {
    return val.rkey_begin();
  }

  template <class Object>
  auto basic_object<Object>::key_end() const -> iterator {
    return val.key_end();
  }

  template <class Object>
  auto basic_object<Object>::rkey_end() const -> reverse_iterator {
    return val.rkey_end();
  }

  template <class Object>
  auto basic_object<Object>::kvbegin() const -> std::tuple<iterator, iterator> {
    return val.kvbegin();
  }

  template <class Object>
  auto basic_object<Object>::kvend() const -> std::tuple<iterator, iterator> {
    return val.kvend();
  }

  template <class Object>
  auto basic_object<Object>::rkvbegin() const -> std::tuple<reverse_iterator, reverse_iterator> {
    return val.rkvbegin();
  }

  template <class Object>
  auto basic_object<Object>::rkvend() const -> std::tuple<reverse_iterator, reverse_iterator> {
    return val.rkvend();
  }

  template <class Object>
  constexpr bool basic_object<Object>::is_view() const noexcept {
    return val.is_view();
  }

  template <class Array>
  constexpr bool basic_array<Array>::is_view() const noexcept {
    return val.is_view();
  }

  template <class String>
  constexpr bool basic_string<String>::is_view() const noexcept {
    return val.is_view();
  }

  template <class Number>
  constexpr bool basic_number<Number>::is_view() const noexcept {
    return val.is_view();
  }

  template <class Boolean>
  constexpr bool basic_flag<Boolean>::is_view() const noexcept {
    return val.is_view();
  }

  template <class Null>
  constexpr bool basic_null<Null>::is_view() const noexcept {
    return val.is_view();
  }

  template <class Object>
  template <bool enabled, class EnableIf>
  auto basic_object<Object>::as_owner() const noexcept {
    using owning_value_t = std::decay_t<decltype(std::declval<value_type>().as_owner())>;
    return basic_object<owning_value_t> {val.as_owner()};
  }

  template <class Array>
  template <bool enabled, class EnableIf>
  auto basic_array<Array>::as_owner() const noexcept {
    using owning_value_t = std::decay_t<decltype(std::declval<value_type>().as_owner())>;
    return basic_array<owning_value_t> {val.as_owner()};
  }

  template <class String>
  template <bool enabled, class EnableIf>
  auto basic_string<String>::as_owner() const noexcept {
    using owning_value_t = std::decay_t<decltype(std::declval<value_type>().as_owner())>;
    return basic_string<owning_value_t> {val.as_owner()};
  }

  template <class Number>
  template <bool enabled, class EnableIf>
  auto basic_number<Number>::as_owner() const noexcept {
    using owning_value_t = std::decay_t<decltype(std::declval<value_type>().as_owner())>;
    return basic_number<owning_value_t> {val.as_owner()};
  }

  template <class Boolean>
  template <bool enabled, class EnableIf>
  auto basic_flag<Boolean>::as_owner() const noexcept {
    using owning_value_t = std::decay_t<decltype(std::declval<value_type>().as_owner())>;
    return basic_flag<owning_value_t> {val.as_owner()};
  }

  template <class Null>
  template <bool enabled, class EnableIf>
  auto basic_null<Null>::as_owner() const noexcept {
    using owning_value_t = std::decay_t<decltype(std::declval<value_type>().as_owner())>;
    return basic_null<owning_value_t> {nullptr};
  }

  inline namespace literals {

    inline string operator ""_dart(char const* val, size_t len) {
      return string {shim::string_view {val, len}};
    }

    inline number operator ""_dart(unsigned long long val) {
      return number {val};
    }

    inline number operator ""_dart(long double val) {
      return number {val};
    }

  }

}

#endif
