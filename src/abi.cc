/*----- Local Includes -----*/

#include "../include/dart/abi.h"

/*----- Macros -----*/

#define DART_RAW_BYTES(input) &((input)->bytes)

#define DEFINE_ABI_UNWRAPPER(input, safe, unsafe)                                           \
  template <class Func>                                                                     \
  dart_err abi_safe_unwrap(Func&& cb, input* pkt) noexcept {                                \
    auto& unwrapped = *reinterpret_cast<safe*>(DART_RAW_BYTES(pkt));                        \
    return err_handler([&] { return std::forward<Func>(cb)(unwrapped); });                  \
  }                                                                                         \
  template <class Func>                                                                     \
  dart_err abi_unsafe_unwrap(Func&& cb, input* pkt) noexcept {                              \
    auto& unwrapped = *reinterpret_cast<unsafe*>(DART_RAW_BYTES(pkt));                      \
    return err_handler([&] { return std::forward<Func>(cb)(unwrapped); });                  \
  }                                                                                         \
  template <class Func>                                                                     \
  dart_err abi_unwrap(Func&& cb, input* pkt) noexcept {                                     \
    /* Figure out what we've been given. */                                                 \
    if (pkt->rtti.rc_id == DART_RC_SAFE) {                                                  \
      return abi_safe_unwrap(std::forward<Func>(cb), pkt);                                  \
    } else if (pkt->rtti.rc_id == DART_RC_UNSAFE) {                                         \
      return abi_unsafe_unwrap(std::forward<Func>(cb), pkt);                                \
    } else {                                                                                \
      return DART_CLIENT_ERROR;                                                             \
    }                                                                                       \
  }                                                                                         \
  template <class Func>                                                                     \
  dart_err abi_unwrap_checked(Func&& cb, input* pkt) noexcept {                             \
    /* Check to make sure the user isn't an idiot */                                        \
    if (!pkt) return DART_CLIENT_ERROR;                                                     \
    return abi_unwrap(std::forward<Func>(cb), pkt);                                         \
  }

#define GENERIC_ABI_FORWARD(cb, ptr, is_const, target_type)                                 \
  generic_abi_unwrap_impl<                                                                  \
    target_type,                                                                            \
    maybe_add_const_t<target_type, is_const>                                                \
  >(std::forward<decltype(cb)>(cb), ptr)

/*----- Globals -----*/

thread_local char const* errmsg = nullptr;

/*----- Private Types -----*/

namespace dart {
  using unsafe_heap = dart::basic_heap<dart::unsafe_ptr>;
  using unsafe_buffer = dart::basic_buffer<dart::unsafe_ptr>;
  using unsafe_packet = dart::basic_packet<dart::unsafe_ptr>;
}

template <class Func>
struct returns_error :
  std::is_same<
    std::result_of_t<Func()>,
    dart_err
  >
{};

template <class Target, bool is_const>
using maybe_add_const_t = std::conditional_t<
  is_const,
  Target const,
  Target
>;

/*----- Private Helper Function Implementations -----*/

namespace {

  template <class Func>
  dart_err call_indirection(Func&& cb, std::true_type) {
    return std::forward<Func>(cb)();
  }
  template <class Func>
  dart_err call_indirection(Func&& cb, std::false_type) {
    std::forward<Func>(cb)();
    return DART_NO_ERROR;
  }

  template <class Func>
  dart_err err_handler(Func&& cb) noexcept try {
    return call_indirection(std::forward<Func>(cb), returns_error<std::decay_t<Func>> {});
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

  DEFINE_ABI_UNWRAPPER(dart_heap, dart::heap, dart::unsafe_heap);
  DEFINE_ABI_UNWRAPPER(dart_heap const, dart::heap const, dart::unsafe_heap const);
  DEFINE_ABI_UNWRAPPER(dart_buffer, dart::buffer, dart::unsafe_buffer);
  DEFINE_ABI_UNWRAPPER(dart_buffer const, dart::buffer const, dart::unsafe_buffer const);
  DEFINE_ABI_UNWRAPPER(dart_packet, dart::packet, dart::unsafe_packet);
  DEFINE_ABI_UNWRAPPER(dart_packet const, dart::packet const, dart::unsafe_packet const);

#undef DEFINE_ABI_UNWRAPPER

  template <class Target, class TargetConst, class Func>
  dart_err generic_abi_unwrap_impl(Func&& cb, void* ptr) {
    auto* punned = reinterpret_cast<Target*>(ptr);
    auto* corrected = const_cast<TargetConst*>(punned);
    return err_handler([&] { return std::forward<Func>(cb)(*corrected); });
  }

  template <bool is_const, class Func>
  dart_err generic_abi_dispatch(Func&& cb, void* ptr) {
    auto* rtti = reinterpret_cast<dart_type_id_t*>(ptr);
    switch (rtti->p_id) {
      case DART_HEAP:
        if (rtti->rc_id == DART_RC_SAFE) {
          return GENERIC_ABI_FORWARD(cb, ptr, is_const, dart::heap);
        } else if (rtti->rc_id == DART_RC_UNSAFE) {
          return GENERIC_ABI_FORWARD(cb, ptr, is_const, dart::unsafe_heap);
        } else {
          return DART_CLIENT_ERROR;
        }
        break;
      case DART_BUFFER:
        if (rtti->rc_id == DART_RC_SAFE) {
          return GENERIC_ABI_FORWARD(cb, ptr, is_const, dart::buffer);
        } else if (rtti->rc_id == DART_RC_UNSAFE) {
          return GENERIC_ABI_FORWARD(cb, ptr, is_const, dart::unsafe_buffer);
        } else {
          return DART_CLIENT_ERROR;
        }
        break;
      case DART_PACKET:
        if (rtti->rc_id == DART_RC_SAFE) {
          return GENERIC_ABI_FORWARD(cb, ptr, is_const, dart::packet);
        } else if (rtti->rc_id == DART_RC_UNSAFE) {
          return GENERIC_ABI_FORWARD(cb, ptr, is_const, dart::unsafe_packet);
        } else {
          return DART_CLIENT_ERROR;
        }
        break;
      default:
        return DART_CLIENT_ERROR;
    }
  }

  template <class Func>
  dart_err generic_abi_unwrap(Func&& cb, void* ptr) {
    return generic_abi_dispatch<false>(std::forward<Func>(cb), ptr);
  }

  template <class Func>
  dart_err generic_abi_unwrap(Func&& cb, void const* ptr) {
    // Don't worry, we'll add the const back before we export.
    return generic_abi_dispatch<true>(std::forward<Func>(cb), const_cast<void*>(ptr));
  }

  template <class Func>
  dart_err generic_abi_unwrap_checked(Func&& cb, void* ptr) {
    if (!ptr) return DART_CLIENT_ERROR;
    return generic_abi_unwrap(std::forward<Func>(cb), ptr);
  }

  template <class Func>
  dart_err generic_abi_unwrap_checked(Func&& cb, void const* ptr) {
    if (!ptr) return DART_CLIENT_ERROR;
    return generic_abi_unwrap(std::forward<Func>(cb), ptr);
  }

}

/*----- Function Implementations -----*/

extern "C" {

  dart_err dart_heap_init(dart_heap* pkt, dart_rc_type rc) {
    // Make sure the user isn't an idiot.
    if (!pkt) return DART_CLIENT_ERROR;

    // Initialize.
    pkt->rtti = {DART_HEAP, rc};
    return err_handler([pkt, rc] {
      if (rc == DART_RC_SAFE) new(pkt) dart::heap();
      else if (rc == DART_RC_UNSAFE) new(pkt) dart::unsafe_heap();
    });
  }

  dart_err dart_heap_copy(dart_heap* dst, dart_heap const* src) {
    // Copy the underlying Dart type.
    auto ret = abi_unwrap_checked(
      dart::shim::compose_together(
        [dst] (dart::heap const& pkt) {
          new(DART_RAW_BYTES(dst)) dart::heap(pkt);
        },
        [dst] (dart::unsafe_heap const& pkt) {
          new(DART_RAW_BYTES(dst)) dart::unsafe_heap(pkt);
        }
      ),
      src
    );

    // Initialize the rest of the structure
    // if we were successful.
    if (!ret) dst->rtti = src->rtti;
    return ret;
  }

  dart_err dart_heap_move(dart_heap* dst, dart_heap* src) {
    // Move the underlying Dart type.
    auto ret = abi_unwrap_checked(
      dart::shim::compose_together(
        [dst] (dart::heap& pkt) {
          new(DART_RAW_BYTES(dst)) dart::heap(std::move(pkt));
        },
        [dst] (dart::unsafe_heap& pkt) {
          new(DART_RAW_BYTES(dst)) dart::unsafe_heap(std::move(pkt));
        }
      ),
      src
    );

    // Initialize the rest of the structure
    // if we were successful.
    if (!ret) dst->rtti = src->rtti;
    return ret;
  }

  dart_err dart_heap_destroy(dart_heap* pkt) {
    return abi_unwrap_checked(
      dart::shim::compose_together(
        [] (dart::heap& pkt) { pkt.~basic_heap(); },
        [] (dart::unsafe_heap& pkt) { pkt.~basic_heap(); }
      ),
      pkt
    );
  }

  dart_err dart_buffer_init(dart_buffer* pkt, dart_rc_type rc) {
    // Make sure the user isn't an idiot.
    if (!pkt) return DART_CLIENT_ERROR;

    // Initialize.
    pkt->rtti = {DART_BUFFER, rc};
    return err_handler([pkt, rc] {
      if (rc == DART_RC_SAFE) new(pkt) dart::buffer();
      else if (rc == DART_RC_UNSAFE) new(pkt) dart::unsafe_buffer();
    });
  }

  dart_err dart_buffer_copy(dart_buffer* dst, dart_buffer const* src) {
    // Copy the underlying Dart type.
    auto ret = abi_unwrap_checked(
      dart::shim::compose_together(
        [dst] (dart::buffer const& pkt) {
          new(DART_RAW_BYTES(dst)) dart::buffer(pkt);
        },
        [dst] (dart::unsafe_buffer const& pkt) {
          new(DART_RAW_BYTES(dst)) dart::unsafe_buffer(pkt);
        }
      ),
      src
    );

    // Initialize the rest of the structure
    // if we were successful.
    if (!ret) dst->rtti = src->rtti;
    return ret;
  }

  dart_err dart_buffer_move(dart_buffer* dst, dart_buffer* src) {
    // Move the underlying Dart type.
    auto ret = abi_unwrap_checked(
      dart::shim::compose_together(
        [dst] (dart::buffer& pkt) {
          new(DART_RAW_BYTES(dst)) dart::buffer(std::move(pkt));
        },
        [dst] (dart::unsafe_buffer& pkt) {
          new(DART_RAW_BYTES(dst)) dart::unsafe_buffer(std::move(pkt));
        }
      ),
      src
    );

    // Initialize the rest of the structure
    // if we were successful.
    if (!ret) dst->rtti = src->rtti;
    return ret;
  }

  dart_err dart_buffer_destroy(dart_buffer* pkt) {
    return abi_unwrap_checked(
      dart::shim::compose_together(
        [] (dart::buffer& pkt) { pkt.~basic_buffer(); },
        [] (dart::unsafe_buffer& pkt) { pkt.~basic_buffer(); }
      ),
      pkt
    );
  }

  dart_err dart_packet_init(dart_packet* pkt, dart_rc_type rc) {
    // Make sure the user isn't an idiot.
    if (!pkt) return DART_CLIENT_ERROR;

    // Initialize.
    pkt->rtti = {DART_PACKET, rc};
    return err_handler([pkt, rc] {
      if (rc == DART_RC_SAFE) new(pkt) dart::packet();
      else if (rc == DART_RC_UNSAFE) new(pkt) dart::unsafe_packet();
    });
  }

  dart_err dart_packet_copy(dart_packet* dst, dart_packet const* src) {
    // Copy the underlying Dart type.
    auto ret = abi_unwrap_checked(
      dart::shim::compose_together(
        [dst] (dart::packet const& pkt) {
          new(DART_RAW_BYTES(dst)) dart::packet(pkt);
        },
        [dst] (dart::unsafe_packet const& pkt) {
          new(DART_RAW_BYTES(dst)) dart::unsafe_packet(pkt);
        }
      ),
      src
    );

    // Initialize the rest of the structure
    // if we were successful.
    if (!ret) dst->rtti = src->rtti;
    return ret;
  }

  dart_err dart_packet_move(dart_packet* dst, dart_packet* src) {
    // Move the underlying Dart type.
    auto ret = abi_unwrap_checked(
      dart::shim::compose_together(
        [dst] (dart::packet& pkt) {
          new(DART_RAW_BYTES(dst)) dart::packet(std::move(pkt));
        },
        [dst] (dart::unsafe_packet& pkt) {
          new(DART_RAW_BYTES(dst)) dart::unsafe_packet(std::move(pkt));
        }
      ),
      src
    );

    // Initialize the rest of the structure
    // if we were successful.
    if (!ret) dst->rtti = src->rtti;
    return ret;
  }

  dart_err dart_packet_destroy(dart_packet* pkt) {
    return abi_unwrap_checked(
      dart::shim::compose_together(
        [] (dart::packet& pkt) { pkt.~basic_packet(); },
        [] (dart::unsafe_packet& pkt) { pkt.~basic_packet(); }
      ),
      pkt
    );
  }

  dart_err dart_destroy(void* pkt) {
    return generic_abi_unwrap_checked(
      dart::shim::compose_together(
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
