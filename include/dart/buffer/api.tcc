#ifndef DART_BUFFER_API_H
#define DART_BUFFER_API_H

/*----- Project Includes -----*/

#include "../common.h"

/*----- Function Implementations -----*/

namespace dart {

  template <template <class> class RefCount>
  basic_buffer<RefCount>::basic_buffer(basic_buffer&& other) noexcept :
    raw(other.raw),
    buffer_ref(std::move(other.buffer_ref))
  {
    other.raw = {detail::raw_type::null, nullptr};
    other.buffer_ref = nullptr;
  }

  template <template <class> class RefCount>
  basic_buffer<RefCount>& basic_buffer<RefCount>::operator =(basic_buffer&& other) & noexcept {
    // Check to make sure we aren't assigning to ourselves.
    if (this == &other) return *this;

    // Defer to destructor and move constructor to keep from copying stateful, easy to screw up,
    // code all over the place.
    this->~basic_buffer();
    new(this) basic_buffer(std::move(other));
    return *this;
  }

  template <template <class> class RefCount>
  template <class KeyType, class EnableIf>
  basic_buffer<RefCount> basic_buffer<RefCount>::operator [](KeyType const& identifier) const& {
    return get(identifier);
  }

  template <template <class> class RefCount>
  template <class KeyType, class EnableIf>
  basic_buffer<RefCount>&& basic_buffer<RefCount>::operator [](KeyType const& identifier) && {
    return std::move(*this).get(identifier);
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::operator ->() noexcept -> basic_buffer* {
    return this;
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::operator ->() const noexcept -> basic_buffer const* {
    return this;
  }

  template <template <class> class RefCount>
  basic_buffer<RefCount>::operator view() const& noexcept {
    view tmp;
    tmp.raw = raw;
    tmp.buffer_ref = typename view::ref_type {buffer_ref.raw()};
    return tmp;
  }

  template <template <class> class RefCount>
  basic_buffer<RefCount>::operator basic_heap<RefCount>() const {
    return convert::cast<basic_heap<RefCount>>(*this);
  }

  template <template <class> class RefCount>
  template <class T, class EnableIf>
  basic_buffer<RefCount>::operator T() const& {
    return convert::cast<T>(*this);
  }

  template <template <class> class RefCount>
  template <class T, class EnableIf>
  basic_buffer<RefCount>::operator T() && {
    return convert::cast<T>(std::move(*this));
  }

  template <template <class> class RefCount>
  template <class T, class EnableIf>
  T basic_buffer<RefCount>::as() const& {
    return convert::cast<T>(*this);
  }

  template <template <class> class RefCount>
  template <class T, class EnableIf>
  decltype(auto) basic_buffer<RefCount>::as() && {
    return convert::cast<T>(std::move(*this));
  }

  template <template <class> class RefCount>
  template <class T, class EnableIf>
  detail::maybe_cast_return_t<T> basic_buffer<RefCount>::maybe_as() const& noexcept {
    using ret_type = detail::maybe_cast_return_t<T>;

    // Attempt the conversion, default construct if something goes wrong.
    // Note that ret_type may be a specialization of shim::optional here.
    try {
      return ret_type {convert::cast<T>(*this)};
    } catch (...) {
      return ret_type {};
    }
  }

  template <template <class> class RefCount>
  template <class T, class EnableIf>
  detail::maybe_cast_return_t<T> basic_buffer<RefCount>::maybe_as() && noexcept {
    using ret_type = detail::maybe_cast_return_t<T>;

    // Attempt the conversion, default construct if something goes wrong.
    // Note that ret_type may be a specialization of shim::optional here.
    try {
      return ret_type {convert::cast<T>(std::move(*this))};
    } catch (...) {
      return ret_type {};
    }
  }

  template <template <class> class RefCount>
  basic_buffer<RefCount> basic_buffer<RefCount>::make_null() noexcept {
    return basic_buffer {};
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_heap<RefCount> basic_buffer<RefCount>::definalize() const {
    return basic_heap<RefCount> {*this};
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_heap<RefCount> basic_buffer<RefCount>::lift() const {
    return definalize();
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount>& basic_buffer<RefCount>::finalize() & {
    return *this;
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount> const& basic_buffer<RefCount>::finalize() const& {
    return *this;
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount>&& basic_buffer<RefCount>::finalize() && {
    return std::move(*this);
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount> const&& basic_buffer<RefCount>::finalize() const&& {
    return std::move(*this);
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount>& basic_buffer<RefCount>::lower() & {
    return finalize();
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount> const& basic_buffer<RefCount>::lower() const& {
    return finalize();
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount>&& basic_buffer<RefCount>::lower() && {
    return std::move(*this).finalize();
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  basic_buffer<RefCount> const&& basic_buffer<RefCount>::lower() const&& {
    return std::move(*this).finalize();
  }

  template <template <class> class RefCount>
  template <template <class> class NewCount>
  basic_buffer<NewCount> basic_buffer<RefCount>::transmogrify(basic_buffer const& buffer) {
    return basic_buffer<NewCount> {buffer.dup_bytes()};
  }

  template <template <class> class RefCount>
  template <class KeyType, class EnableIf>
  basic_buffer<RefCount> basic_buffer<RefCount>::get(KeyType const& identifier) const& {
    switch (identifier.get_type()) {
      case type::string:
        return get(identifier.strv());
      case type::integer:
        return get(identifier.integer());
      default:
        throw type_error("dart::buffer cannot retrieve values with non-string/integer type.");
    }
  }

  template <template <class> class RefCount>
  template <class KeyType, class EnableIf>
  basic_buffer<RefCount>&& basic_buffer<RefCount>::get(KeyType const& identifier) && {
    switch (identifier.get_type()) {
      case type::string:
        return std::move(*this).get(identifier.strv());
      case type::integer:
        return std::move(*this).get(identifier.integer());
      default:
        throw type_error("dart::buffer cannot retrieve values with non-string/integer type.");
    }
  }

  template <template <class> class RefCount>
  template <class KeyType, class EnableIf>
  basic_buffer<RefCount> basic_buffer<RefCount>::at(KeyType const& identifier) const& {
    switch (identifier.at_type()) {
      case type::string:
        return at(identifier.strv());
      case type::integer:
        return at(identifier.integer());
      default:
        throw type_error("dart::buffer cannot retrieve values with non-string/integer type.");
    }
  }

  template <template <class> class RefCount>
  template <class KeyType, class EnableIf>
  basic_buffer<RefCount>&& basic_buffer<RefCount>::at(KeyType const& identifier) && {
    switch (identifier.at_type()) {
      case type::string:
        return std::move(*this).at(identifier.strv());
      case type::integer:
        return std::move(*this).at(identifier.integer());
      default:
        throw type_error("dart::buffer cannot retrieve values with non-string/integer type.");
    }
  }

  template <template <class> class RefCount>
  std::vector<basic_buffer<RefCount>> basic_buffer<RefCount>::values() const {
    return detail::values_impl(*this);
  }

  template <template <class> class RefCount>
  gsl::span<gsl::byte const> basic_buffer<RefCount>::get_bytes() const {
    if (!is_object()) throw type_error("dart::buffer is not an object and cannot return a network buffer");
    auto len = detail::find_sizeof<RefCount>({detail::raw_type::object, raw.buffer});
    return gsl::make_span(raw.buffer, len);
  }

  template <template <class> class RefCount>
  size_t basic_buffer<RefCount>::share_bytes(RefCount<gsl::byte const>& bytes) const {
    if (is_null()) throw type_error("dart::buffer is null and has no network buffer");

    // Destroy and initialize the given reference counter instance as a copy of our buffer.
    buffer_ref.share(bytes);

    // Return the size of the packet.
    return detail::find_sizeof<RefCount>({detail::raw_type::object, buffer_ref.get()});
  }

  template <template <class> class RefCount>
  std::unique_ptr<gsl::byte const[], void (*) (gsl::byte const*)> basic_buffer<RefCount>::dup_bytes() const {
    size_type dummy;
    return dup_bytes(dummy);
  }

  template <template <class> class RefCount>
  std::unique_ptr<gsl::byte const[], void (*) (gsl::byte const*)> basic_buffer<RefCount>::dup_bytes(size_type& len) const {
    using tmp_ptr = std::unique_ptr<gsl::byte const[], void (*) (gsl::byte const*)>;

    auto buf = get_bytes();
    len = buf.size();
    return detail::aligned_alloc<RefCount, tmp_ptr>(buf.size(), detail::raw_type::object, [&] (auto* dup) {
      std::copy(std::begin(buf), std::end(buf), dup);
    });
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::size() const -> size_type {
    // Validate sanity.
    if (!is_aggregate() && !is_str()) {
      throw type_error("dart::buffer is a primitive, and has no size.");
    }

    // Get our size.
    if (is_aggregate()) {
      return detail::aggregate_deref<RefCount>([] (auto& aggr) { return aggr.size(); }, raw);
    } else {
      return detail::string_deref([] (auto& str) { return str.size(); }, raw);
    }
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::empty() const {
    return size() == 0ULL;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_object() const noexcept {
    return detail::simplify_type(raw.type) == type::object;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_array() const noexcept {
    return detail::simplify_type(raw.type) == type::array;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_aggregate() const noexcept {
    return is_object() || is_array();
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_str() const noexcept {
    return detail::simplify_type(raw.type) == type::string;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_integer() const noexcept {
    return detail::simplify_type(raw.type) == type::integer;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_decimal() const noexcept {
    return detail::simplify_type(raw.type) == type::decimal;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_numeric() const noexcept {
    return is_integer() || is_decimal();
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_boolean() const noexcept {
    return detail::simplify_type(raw.type) == type::boolean;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_null() const noexcept {
    return detail::simplify_type(raw.type) == type::null;
  }

  template <template <class> class RefCount>
  bool basic_buffer<RefCount>::is_primitive() const noexcept {
    return !is_object() && !is_array() && !is_null();
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::get_type() const noexcept -> type {
    return detail::simplify_type(raw.type);
  }

  template <template <class> class RefCount>
  constexpr bool basic_buffer<RefCount>::is_finalized() const noexcept {
    return true;
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::refcount() const noexcept -> size_type {
    return buffer_ref.use_count();
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::begin() const -> iterator {
    // Construct an iterator that knows how to find its memory.
    auto impl = detail::aggregate_deref<RefCount>([] (auto& aggr) { return aggr.begin(); }, raw);
    return iterator(*this, impl);
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::cbegin() const -> iterator {
    return begin();
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::end() const -> iterator {
    // Construct an iterator that knows how to find its memory.
    auto impl = detail::aggregate_deref<RefCount>([] (auto& aggr) { return aggr.end(); }, raw);
    return iterator(*this, impl);
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::cend() const -> iterator {
    return end();
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::rbegin() const -> reverse_iterator {
    return reverse_iterator {end()};
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::rend() const -> reverse_iterator {
    return reverse_iterator {begin()};
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::key_begin() const -> iterator {
    return iterator(*this, detail::get_object<RefCount>(raw)->key_begin());
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::rkey_begin() const -> reverse_iterator {
    return reverse_iterator {key_end()};
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::key_end() const -> iterator {
    return iterator(*this, detail::get_object<RefCount>(raw)->key_end());
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::rkey_end() const -> reverse_iterator {
    return reverse_iterator {key_begin()};
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::kvbegin() const -> std::tuple<iterator, iterator> {
    return std::tuple<iterator, iterator> {key_begin(), begin()};
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::kvend() const -> std::tuple<iterator, iterator> {
    return std::tuple<iterator, iterator> {key_end(), end()};
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::rkvbegin() const -> std::tuple<reverse_iterator, reverse_iterator> {
    return std::tuple<reverse_iterator, reverse_iterator> {rkey_begin(), rbegin()};
  }

  template <template <class> class RefCount>
  auto basic_buffer<RefCount>::rkvend() const -> std::tuple<reverse_iterator, reverse_iterator> {
    return std::tuple<reverse_iterator, reverse_iterator> {rkey_end(), rend()};
  }

  template <template <class> class RefCount>
  constexpr bool basic_buffer<RefCount>::is_view() const noexcept {
    return !refcount::is_owner<RefCount>::value;
  }

  template <template <class> class RefCount>
  template <bool enabled, class EnableIf>
  auto basic_buffer<RefCount>::as_owner() const noexcept {
    refcount::owner_indirection_t<dart::basic_buffer, RefCount> tmp;
    tmp.raw = raw;
    if (buffer_ref) tmp.buffer_ref = buffer_ref.raw().raw();
    return tmp;
  }

}

#endif
