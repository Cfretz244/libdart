/*----- Local Includes -----*/

#include "../include/dart/abi.h"

/*----- Build Sanity Checks -----*/

static_assert(sizeof(dart::heap) <= DART_HEAP_MAX_SIZE, "Dart ABI is misconfigured");
static_assert(sizeof(dart::buffer) <= DART_BUFFER_MAX_SIZE, "Dart ABI is misconfigured");
static_assert(sizeof(dart::packet) <= DART_PACKET_MAX_SIZE, "Dart ABI is misconfigured");

/*----- Macros -----*/

#define DART_RAW_TYPE(input) (input)->rtti.p_id
#define DART_RAW_BYTES(input) &((input)->bytes)

/*----- Globals -----*/

thread_local char const* errmsg = nullptr;

/*----- Private Types -----*/

namespace dart {
  using unsafe_heap = dart::basic_heap<dart::unsafe_ptr>;
  using unsafe_buffer = dart::basic_buffer<dart::unsafe_ptr>;
  using unsafe_packet = dart::basic_packet<dart::unsafe_ptr>;
}

template <class Func, class... Args>
struct is_callable_impl {

  private:

    struct nonesuch {};

  public:

    template <class F, class... A>
    static decltype(std::declval<F>()(std::declval<A>()...)) detect(int);
    template <class F, class... A>
    static nonesuch detect(...);

    using type = decltype(detect<Func, Args...>(0));
    using result_type = std::integral_constant<bool, !std::is_same<type, nonesuch>::value>;

};

template <class Func, class... Args>
struct is_callable : is_callable_impl<Func, Args...>::result_type {
  using type = typename is_callable_impl<Func, Args...>::type;
};
template <class Func, class... Args>
using is_callable_t = typename is_callable<Func, Args...>::type;

template <class Func, class... Args>
struct returns_error :
  std::is_same<
    is_callable_t<Func, Args...>,
    dart_err
  >
{};

template <class Target, bool is_const>
using maybe_const_t = std::conditional_t<
  is_const,
  Target const,
  Target
>;

/*----- Private Helper Function Implementations -----*/

namespace {

  template <class... Funcs>
  auto compose(Funcs&&... fs) {
    return dart::shim::compose_together(std::forward<Funcs>(fs)...);
  }

  template <class Func, class... Args>
  dart_err call_indirection(std::true_type, Func&& cb, Args&&... the_args) {
    return std::forward<Func>(cb)(std::forward<Args>(the_args)...);
  }
  template <class Func, class... Args>
  dart_err call_indirection(std::false_type, Func&& cb, Args&&... the_args) {
    std::forward<Func>(cb)(std::forward<Args>(the_args)...);
    return DART_NO_ERROR;
  }

  template <class Func, class... Args>
  dart_err safe_call(Func&& cb, Args&&... the_args) {
    return compose(
      [] (std::true_type, auto&& c, auto&&... as) {
        return call_indirection(returns_error<Func, Args...> {},
            std::forward<decltype(c)>(c), std::forward<decltype(as)>(as)...);
      },
      [] (std::false_type, auto&&, auto&&) {
        return DART_CLIENT_ERROR;
      }
    )(is_callable<Func, Args...> {}, std::forward<Func>(cb), std::forward<Args>(the_args)...);
  }

  template <bool is_const, class Func>
  dart_err heap_unwrap_impl(Func&& cb, dart_heap_t* pkt) {
    switch (pkt->rtti.rc_id) {
      case DART_RC_SAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::heap*>(DART_RAW_BYTES(pkt));
          return safe_call(std::forward<Func>(cb),
              const_cast<maybe_const_t<dart::heap, is_const>&>(*rt_ptr));
        }
      case DART_RC_UNSAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::unsafe_heap*>(DART_RAW_BYTES(pkt));
          return safe_call(std::forward<Func>(cb),
              const_cast<maybe_const_t<dart::unsafe_heap, is_const>&>(*rt_ptr));
        }
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <bool is_const, class Func>
  dart_err buffer_unwrap_impl(Func&& cb, dart_buffer_t* pkt) {
    switch (pkt->rtti.rc_id) {
      case DART_RC_SAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::buffer*>(DART_RAW_BYTES(pkt));
          return safe_call(std::forward<Func>(cb),
              const_cast<maybe_const_t<dart::buffer, is_const>&>(*rt_ptr));
        }
      case DART_RC_UNSAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::unsafe_buffer*>(DART_RAW_BYTES(pkt));
          return safe_call(std::forward<Func>(cb),
              const_cast<maybe_const_t<dart::unsafe_buffer, is_const>&>(*rt_ptr));
        }
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <bool is_const, class Func>
  dart_err packet_unwrap_impl(Func&& cb, dart_packet_t* pkt) {
    switch (pkt->rtti.rc_id) {
      case DART_RC_SAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::packet*>(DART_RAW_BYTES(pkt));
          return safe_call(std::forward<Func>(cb),
              const_cast<maybe_const_t<dart::packet, is_const>&>(*rt_ptr));
        }
      case DART_RC_UNSAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::unsafe_packet*>(DART_RAW_BYTES(pkt));
          return safe_call(std::forward<Func>(cb),
              const_cast<maybe_const_t<dart::unsafe_packet, is_const>&>(*rt_ptr));
        }
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <bool is_const, class Func>
  dart_err generic_unwrap_impl(Func&& cb, void* pkt) {
    auto* rtti = reinterpret_cast<dart_type_id_t*>(pkt);
    switch (rtti->p_id) {
      case DART_HEAP:
        return heap_unwrap_impl<is_const>(std::forward<Func>(cb), reinterpret_cast<dart_heap_t*>(pkt));
      case DART_BUFFER:
        return buffer_unwrap_impl<is_const>(std::forward<Func>(cb), reinterpret_cast<dart_buffer_t*>(pkt));
      case DART_PACKET:
        return packet_unwrap_impl<is_const>(std::forward<Func>(cb), reinterpret_cast<dart_packet_t*>(pkt));
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <class Func>
  dart_err generic_unwrap(Func&& cb, void* pkt) {
    return generic_unwrap_impl<false>(std::forward<Func>(cb), pkt);
  }

  template <class Func>
  dart_err generic_unwrap(Func&& cb, void const* pkt) {
    return generic_unwrap_impl<true>(std::forward<Func>(cb), const_cast<void*>(pkt));
  }

  template <class Func>
  dart_err heap_unwrap(Func&& cb, dart_heap_t* pkt) {
    return heap_unwrap_impl<false>(std::forward<Func>(cb), pkt);
  }

  template <class Func>
  dart_err heap_unwrap(Func&& cb, dart_heap_t const* pkt) {
    return heap_unwrap_impl<true>(std::forward<Func>(cb), const_cast<dart_heap_t*>(pkt));
  }

  template <class Func>
  dart_err heap_construct(Func&& cb, dart_heap_t* pkt) {
    switch (pkt->rtti.rc_id) {
      case DART_RC_SAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::heap*>(DART_RAW_BYTES(pkt));
          auto ret = safe_call(std::forward<Func>(cb), rt_ptr);
          return ret ? ret : DART_NO_ERROR;
        }
      case DART_RC_UNSAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::unsafe_heap*>(DART_RAW_BYTES(pkt));
          auto ret = safe_call(std::forward<Func>(cb), rt_ptr);
          return ret ? ret : DART_NO_ERROR;
        }
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <class Func>
  dart_err buffer_unwrap(Func&& cb, dart_buffer_t* pkt) {
    return buffer_unwrap_impl<false>(std::forward<Func>(cb), pkt);
  }

  template <class Func>
  dart_err buffer_unwrap(Func&& cb, dart_buffer_t const* pkt) {
    return buffer_unwrap_impl<true>(std::forward<Func>(cb), const_cast<dart_buffer_t*>(pkt));
  }

  template <class Func>
  dart_err buffer_construct(Func&& cb, dart_buffer_t* pkt) {
    switch (pkt->rtti.rc_id) {
      case DART_RC_SAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::buffer*>(DART_RAW_BYTES(pkt));
          auto ret = safe_call(std::forward<Func>(cb), rt_ptr);
          return ret ? ret : DART_NO_ERROR;
        }
      case DART_RC_UNSAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::unsafe_buffer*>(DART_RAW_BYTES(pkt));
          auto ret = safe_call(std::forward<Func>(cb), rt_ptr);
          return ret ? ret : DART_NO_ERROR;
        }
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <class Func>
  dart_err packet_unwrap(Func&& cb, dart_packet_t* pkt) {
    return packet_unwrap_impl<false>(std::forward<Func>(cb), pkt);
  }

  template <class Func>
  dart_err packet_unwrap(Func&& cb, dart_packet_t const* pkt) {
    return packet_unwrap_impl<true>(std::forward<Func>(cb), const_cast<dart_packet_t*>(pkt));
  }

  template <class Func>
  dart_err packet_construct(Func&& cb, dart_packet_t* pkt) {
    switch (pkt->rtti.rc_id) {
      case DART_RC_SAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::packet*>(DART_RAW_BYTES(pkt));
          auto ret = safe_call(std::forward<Func>(cb), rt_ptr);
          return ret ? ret : DART_NO_ERROR;
        }
      case DART_RC_UNSAFE:
        {
          auto* rt_ptr = reinterpret_cast<dart::unsafe_packet*>(DART_RAW_BYTES(pkt));
          auto ret = safe_call(std::forward<Func>(cb), rt_ptr);
          return ret ? ret : DART_NO_ERROR;
        }
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <class Func>
  dart_err err_handler(Func&& cb) noexcept try {
    return std::forward<Func>(cb)();
  } catch (dart::type_error const& err) {
    errmsg = err.what();
    return DART_TYPE_ERROR;
  } catch (dart::state_error const& err) {
    errmsg = err.what();
    return DART_STATE_ERROR;
  } catch (dart::parse_error const& err) {
    errmsg = err.what();
    return DART_PARSE_ERROR;
  } catch (std::logic_error const& err) {
    errmsg = err.what();
    return DART_LOGIC_ERROR;
  } catch (std::runtime_error const& err) {
    errmsg = err.what();
    return DART_RUNTIME_ERROR;
  } catch (...) {
    errmsg = "Dart caught an unexpected error type. This is a bug, please make a report";
    return DART_UNKNOWN_ERROR;
  }

  template <class Func, class Ptr>
  dart_err generic_access(Func&& cb, Ptr* pkt) noexcept {
    return err_handler([&cb, pkt] { return generic_unwrap(std::forward<Func>(cb), pkt); });
  }

  template <class Func, class Ptr>
  dart_err heap_access(Func&& cb, Ptr* pkt) noexcept {
    return err_handler([&cb, pkt] { return heap_unwrap(std::forward<Func>(cb), pkt); });
  }

  template <class Func>
  dart_err heap_constructor_access(Func&& cb, dart_heap_t* pkt) noexcept {
    return err_handler([&cb, pkt] { return heap_construct(std::forward<Func>(cb), pkt); });
  }

  template <class Func>
  dart_err heap_typed_constructor_access(Func&& cb, dart_heap_t* pkt, dart_rc_type rc) noexcept {
    // Default construct our heap.
    auto ret = dart_heap_init_rc(pkt, rc);
    if (ret) return ret;

    // Assign to it.
    return err_handler([&cb, pkt] { return heap_unwrap(std::forward<Func>(cb), pkt); });
  }

  template <class Func, class Ptr>
  dart_err buffer_access(Func&& cb, Ptr* pkt) noexcept {
    return err_handler([&cb, pkt] { return buffer_unwrap(std::forward<Func>(cb), pkt); });
  }

  template <class Func>
  dart_err buffer_constructor_access(Func&& cb, dart_buffer_t* pkt) noexcept {
    return err_handler([&cb, pkt] { return buffer_construct(std::forward<Func>(cb), pkt); });
  }

  template <class Func, class Ptr>
  dart_err packet_access(Func&& cb, Ptr* pkt) noexcept {
    return err_handler([&cb, pkt] { return packet_unwrap(std::forward<Func>(cb), pkt); });
  }

  template <class Func>
  dart_err packet_constructor_access(Func&& cb, dart_packet_t* pkt) noexcept {
    return err_handler([&cb, pkt] { return packet_construct(std::forward<Func>(cb), pkt); });
  }

}

/*----- Function Implementations -----*/

extern "C" {

  /*----- Dart Heap Functions -----*/

  dart_err dart_heap_init(dart_heap_t* pkt) {
    return dart_heap_init_rc(pkt, DART_RC_SAFE);
  }

  dart_err dart_heap_init_rc(dart_heap_t* pkt, dart_rc_type rc) {
    // Make sure the user isn't an idiot.
    if (!pkt) return DART_CLIENT_ERROR;

    // Initialize.
    pkt->rtti = {DART_HEAP, rc};
    return heap_constructor_access(
      compose(
        [] (dart::heap* ptr) { new(ptr) dart::heap(); },
        [] (dart::unsafe_heap* ptr) { new(ptr) dart::unsafe_heap(); }
      ),
      pkt
    );
  }

  dart_err dart_heap_copy(dart_heap_t* dst, dart_heap_t const* src) {
    // Make sure the user isn't an idiot.
    if (!dst || !src) return DART_CLIENT_ERROR;

    // Initialize.
    dst->rtti = src->rtti;
    return heap_access(
      compose(
        [dst] (dart::heap const& src) {
          return heap_construct([&src] (dart::heap* dst) { new(dst) dart::heap(src); }, dst);
        },
        [dst] (dart::unsafe_heap const& src) {
          return heap_construct([&src] (dart::unsafe_heap* dst) { new(dst) dart::unsafe_heap(src); }, dst);
        }
      ),
      src
    );
  }

  dart_err dart_heap_move(dart_heap_t* dst, dart_heap_t* src) {
    // Make sure the user isn't an idiot.
    if (!dst || !src) return DART_CLIENT_ERROR;

    // Initialize.
    dst->rtti = src->rtti;
    return heap_access(
      compose(
        [dst] (dart::heap& src) {
          return heap_construct([&src] (dart::heap* dst) { new(dst) dart::heap(std::move(src)); }, dst);
        },
        [dst] (dart::unsafe_heap& src) {
          return heap_construct([&src] (dart::unsafe_heap* dst) { new(dst) dart::unsafe_heap(std::move(src)); }, dst);
        }
      ),
      src
    );
  }

  dart_err dart_heap_destroy(dart_heap_t* pkt) {
    // Destroy.
    return heap_access(
      compose(
        [] (dart::heap& pkt) { pkt.~basic_heap(); },
        [] (dart::unsafe_heap& pkt) { pkt.~basic_heap(); }
      ),
      pkt
    );
  }

  dart_err dart_heap_init_obj(dart_heap_t* pkt) {
    return dart_heap_init_obj_rc(pkt, DART_RC_SAFE);
  }

  dart_err dart_heap_init_obj_rc(dart_heap_t* pkt, dart_rc_type rc) {
    // Default initialize, then assign.
    return heap_typed_constructor_access(
      compose(
        [] (dart::heap& pkt) { pkt = dart::heap::make_object(); },
        [] (dart::unsafe_heap& pkt) { pkt = dart::unsafe_heap::make_object(); }
      ),
      pkt,
      rc
    );
  }

  dart_err dart_heap_init_arr(dart_heap_t* pkt) {
    return dart_heap_init_arr_rc(pkt, DART_RC_SAFE);
  }

  dart_err dart_heap_init_arr_rc(dart_heap_t* pkt, dart_rc_type rc) {
    // Default initialize, then assign.
    return heap_typed_constructor_access(
      compose(
        [] (dart::heap& pkt) { pkt = dart::heap::make_array(); },
        [] (dart::unsafe_heap& pkt) { pkt = dart::unsafe_heap::make_array(); }
      ),
      pkt,
      rc
    );
  }

  dart_err dart_heap_init_str(dart_heap_t* pkt, char const* str, size_t len) {
    return dart_heap_init_str_rc(pkt, DART_RC_SAFE, str, len);
  }

  dart_err dart_heap_init_str_rc(dart_heap_t* pkt, dart_rc_type rc, char const* str, size_t len) {
    // Default initialize, then assign
    return heap_typed_constructor_access(
      compose(
        [str, len] (dart::heap& pkt) { pkt = dart::heap::make_string({str, len}); },
        [str, len] (dart::unsafe_heap& pkt) { pkt = dart::unsafe_heap::make_string({str, len}); }
      ),
      pkt,
      rc
    );
  }

  dart_err dart_heap_init_int(dart_heap_t* pkt, int64_t val) {
    return dart_heap_init_int_rc(pkt, DART_RC_SAFE, val);
  }

  dart_err dart_heap_init_int_rc(dart_heap_t* pkt, dart_rc_type rc, int64_t val) {
    // Default initialize, then assign
    return heap_typed_constructor_access(
      compose(
        [val] (dart::heap& pkt) { pkt = dart::heap::make_integer(val); },
        [val] (dart::unsafe_heap& pkt) { pkt = dart::unsafe_heap::make_integer(val); }
      ),
      pkt,
      rc
    );
  }

  dart_err dart_heap_init_dcm(dart_heap_t* pkt, double val) {
    return dart_heap_init_dcm_rc(pkt, DART_RC_SAFE, val);
  }

  dart_err dart_heap_init_dcm_rc(dart_heap_t* pkt, dart_rc_type rc, double val) {
    // Default initialize, then assign
    return heap_typed_constructor_access(
      compose(
        [val] (dart::heap& pkt) { pkt = dart::heap::make_decimal(val); },
        [val] (dart::unsafe_heap& pkt) { pkt = dart::unsafe_heap::make_decimal(val); }
      ),
      pkt,
      rc
    );
  }

  dart_err dart_heap_init_bool(dart_heap_t* pkt, int val) {
    return dart_heap_init_bool_rc(pkt, DART_RC_SAFE, val);
  }

  dart_err dart_heap_init_bool_rc(dart_heap_t* pkt, dart_rc_type rc, int val) {
    // Default initialize, then assign
    return heap_typed_constructor_access(
      compose(
        [val] (dart::heap& pkt) { pkt = dart::heap::make_boolean(val); },
        [val] (dart::unsafe_heap& pkt) { pkt = dart::unsafe_heap::make_boolean(val); }
      ),
      pkt,
      rc
    );
  }

  dart_err dart_heap_init_null(dart_heap_t* pkt) {
    return dart_heap_init_null_rc(pkt, DART_RC_SAFE);
  }

  dart_err dart_heap_init_null_rc(dart_heap_t* pkt, dart_rc_type rc) {
    // Default initialize, then assign.
    // Unnecessary, but done for consistency of code formatting.
    return heap_typed_constructor_access(
      compose(
        [] (dart::heap& pkt) { pkt = dart::heap::make_null(); },
        [] (dart::unsafe_heap& pkt) { pkt = dart::unsafe_heap::make_null(); }
      ),
      pkt,
      rc
    );
  }

  dart_err dart_buffer_init(dart_buffer_t* pkt) {
    return dart_buffer_init_rc(pkt, DART_RC_SAFE);
  }

  dart_err dart_buffer_init_rc(dart_buffer_t* pkt, dart_rc_type rc) {
    // Make sure the user isn't an idiot.
    if (!pkt) return DART_CLIENT_ERROR;

    // Initialize.
    pkt->rtti = {DART_HEAP, rc};
    return buffer_constructor_access(
      compose(
        [] (dart::buffer* ptr) { new(ptr) dart::buffer(); },
        [] (dart::unsafe_buffer* ptr) { new(ptr) dart::unsafe_buffer(); }
      ),
      pkt
    );
  }

  dart_err dart_buffer_copy(dart_buffer_t* dst, dart_buffer_t const* src) {
    // Make sure the user isn't an idiot.
    if (!dst || !src) return DART_CLIENT_ERROR;

    // Initialize.
    dst->rtti = src->rtti;
    return buffer_access(
      compose(
        [dst] (dart::buffer const& src) {
          return buffer_construct([&src] (dart::buffer* dst) { new(dst) dart::buffer(src); }, dst);
        },
        [dst] (dart::unsafe_buffer const& src) {
          return buffer_construct([&src] (dart::unsafe_buffer* dst) { new(dst) dart::unsafe_buffer(src); }, dst);
        }
      ),
      src
    );
  }

  dart_err dart_buffer_move(dart_buffer_t* dst, dart_buffer_t* src) {
    // Make sure the user isn't an idiot.
    if (!dst || !src) return DART_CLIENT_ERROR;

    // Initialize.
    dst->rtti = src->rtti;
    return buffer_access(
      compose(
        [dst] (dart::buffer& src) {
          return buffer_construct([&src] (dart::buffer* dst) { new(dst) dart::buffer(std::move(src)); }, dst);
        },
        [dst] (dart::unsafe_buffer& src) {
          return buffer_construct([&src] (dart::unsafe_buffer* dst) { new(dst) dart::unsafe_buffer(std::move(src)); }, dst);
        }
      ),
      src
    );
  }

  dart_err dart_buffer_destroy(dart_buffer_t* pkt) {
    return buffer_access(
      compose(
        [] (dart::buffer& pkt) { pkt.~basic_buffer(); },
        [] (dart::unsafe_buffer& pkt) { pkt.~basic_buffer(); }
      ),
      pkt
    );
  }

  dart_err dart_packet_init(dart_packet_t* pkt) {
    return dart_packet_init_rc(pkt, DART_RC_SAFE);
  }

  dart_err dart_packet_init_rc(dart_packet_t* pkt, dart_rc_type rc) {
    // Make sure the user isn't an idiot.
    if (!pkt) return DART_CLIENT_ERROR;

    // Initialize.
    pkt->rtti = {DART_HEAP, rc};
    return packet_constructor_access(
      compose(
        [] (dart::packet* ptr) { new(ptr) dart::packet(); },
        [] (dart::unsafe_packet* ptr) { new(ptr) dart::unsafe_packet(); }
      ),
      pkt
    );
  }

  dart_err dart_packet_copy(dart_packet_t* dst, dart_packet_t const* src) {
    // Make sure the user isn't an idiot.
    if (!dst || !src) return DART_CLIENT_ERROR;

    // Initialize.
    dst->rtti = src->rtti;
    return packet_access(
      compose(
        [dst] (dart::packet const& src) {
          return packet_construct([&src] (dart::packet* dst) { new(dst) dart::packet(src); }, dst);
        },
        [dst] (dart::unsafe_packet const& src) {
          return packet_construct([&src] (dart::unsafe_packet* dst) { new(dst) dart::unsafe_packet(src); }, dst);
        }
      ),
      src
    );
  }

  dart_err dart_packet_move(dart_packet_t* dst, dart_packet_t* src) {
    // Make sure the user isn't an idiot.
    if (!dst || !src) return DART_CLIENT_ERROR;

    // Initialize.
    dst->rtti = src->rtti;
    return packet_access(
      compose(
        [dst] (dart::packet& src) {
          return packet_construct([&src] (dart::packet* dst) { new(dst) dart::packet(std::move(src)); }, dst);
        },
        [dst] (dart::unsafe_packet& src) {
          return packet_construct([&src] (dart::unsafe_packet* dst) { new(dst) dart::unsafe_packet(std::move(src)); }, dst);
        }
      ),
      src
    );
  }

  dart_err dart_packet_destroy(dart_packet_t* pkt) {
    return packet_access(
      compose(
        [] (dart::packet& pkt) { pkt.~basic_packet(); },
        [] (dart::unsafe_packet& pkt) { pkt.~basic_packet(); }
      ),
      pkt
    );
  }

  dart_err dart_destroy(void* pkt) {
    // Generic destroy function.
    // Get rid of it whatever it is.
    return generic_access(
      compose(
        [] (dart::heap& pkt) { pkt.~basic_heap(); },
        [] (dart::unsafe_heap& pkt) { pkt.~basic_heap(); },
        [] (dart::buffer& pkt) { pkt.~basic_buffer(); },
        [] (dart::unsafe_buffer& pkt) { pkt.~basic_buffer(); },
        [] (dart::packet& pkt) { pkt.~basic_packet(); },
        [] (dart::unsafe_packet& pkt) { pkt.~basic_packet(); }
      ),
      pkt
    );
  }

  char const* dart_get_error() {
    return errmsg;
  }

}
