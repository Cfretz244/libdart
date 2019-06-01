#ifndef DART_ABI_H
#define DART_ABI_H

/*----- Local Includes -----*/

#include "../dart.h"

/*----- Macros ------*/

#define DART_BUFFER_MAX_SIZE  (1U << 5U)
#define DART_HEAP_MAX_SIZE    (1U << 6U)
#define DART_PACKET_MAX_SIZE  DART_HEAP_MAX_SIZE

#ifdef __cplusplus
extern "C" {
#endif

  /*----- Public Type Declarations -----*/

  enum dart_type {
    DART_OBJECT,
    DART_ARRAY,
    DART_STRING,
    DART_INTEGER,
    DART_DECIMAL,
    DART_BOOLEAN,
    DART_NULL
  };

  enum dart_packet_type {
    DART_HEAP,
    DART_BUFFER,
    DART_PACKET
  };

  enum dart_rc_type {
    DART_RC_SAFE,
    DART_RC_UNSAFE
  };

  enum dart_err {
    DART_NO_ERROR = 0,
    DART_TYPE_ERROR,
    DART_LOGIC_ERROR,
    DART_STATE_ERROR,
    DART_PARSE_ERROR,
    DART_RUNTIME_ERROR,
    DART_CLIENT_ERROR,
    DART_UNKNOWN_ERROR
  };

  struct dart_type_id {
    dart_packet_type p_id;
    dart_rc_type rc_id;
  };
  typedef struct dart_type_id dart_type_id_t;

  struct dart_heap {
    dart_type_id_t rtti;
    char bytes[DART_HEAP_MAX_SIZE];
  };
  typedef struct dart_heap dart_heap_t;

  struct dart_buffer {
    dart_type_id_t rtti;
    char bytes[DART_BUFFER_MAX_SIZE];
  };
  typedef struct dart_buffer dart_buffer_t;

  struct dart_packet {
    dart_type_id_t rtti;
    char bytes[DART_PACKET_MAX_SIZE];
  };
  typedef struct dart_packet dart_packet_t;

  struct dart_string_view {
    char const* ptr;
    size_t len;
  };
  typedef struct dart_string_view dart_string_view_t;

  /*----- Public Function Declarations -----*/

  // dart::heap lifecycle functions.
  dart_err dart_heap_init(dart_heap_t* pkt);
  dart_err dart_heap_init_rc(dart_heap_t* pkt, dart_rc_type rc);
  dart_err dart_heap_copy(dart_heap_t* dst, dart_heap_t const* src);
  dart_err dart_heap_move(dart_heap_t* dst, dart_heap_t* src);
  dart_err dart_heap_destroy(dart_heap_t* pkt);

  // dart::heap typed constructors.
  dart_err dart_heap_init_obj(dart_heap_t* pkt);
  dart_err dart_heap_init_obj_rc(dart_heap_t* pkt, dart_rc_type rc);
  dart_err dart_heap_init_arr(dart_heap_t* pkt);
  dart_err dart_heap_init_arr_rc(dart_heap_t* pkt, dart_rc_type rc);
  dart_err dart_heap_init_str(dart_heap_t* pkt, char const* str, size_t len);
  dart_err dart_heap_init_str_rc(dart_heap_t* pkt, dart_rc_type rc, char const* str, size_t len);
  dart_err dart_heap_init_int(dart_heap_t* pkt, int64_t val);
  dart_err dart_heap_init_int_rc(dart_heap_t* pkt, dart_rc_type rc, int64_t val);
  dart_err dart_heap_init_dcm(dart_heap_t* pkt, double val);
  dart_err dart_heap_init_dcm_rc(dart_heap_t* pkt, dart_rc_type rc, double val);
  dart_err dart_heap_init_bool(dart_heap_t* pkt, int val);
  dart_err dart_heap_init_bool_rc(dart_heap_t* pkt, dart_rc_type rc, int val);
  dart_err dart_heap_init_null(dart_heap_t* pkt);
  dart_err dart_heap_init_null_rc(dart_heap_t* pkt, dart_rc_type rc);

  // dart::buffer lifecycle functions.
  dart_err dart_buffer_init(dart_buffer_t* pkt);
  dart_err dart_buffer_init_rc(dart_buffer_t* pkt, dart_rc_type rc);
  dart_err dart_buffer_copy(dart_buffer_t* dst, dart_buffer_t const* src);
  dart_err dart_buffer_move(dart_buffer_t* dst, dart_buffer_t* src);
  dart_err dart_buffer_destroy(dart_buffer_t* pkt);

  // dart::packet lifecycle functions.
  dart_err dart_packet_init(dart_packet_t* pkt);
  dart_err dart_packet_init_rc(dart_packet_t* pkt, dart_rc_type rc);
  dart_err dart_packet_copy(dart_packet_t* dst, dart_packet_t const* src);
  dart_err dart_packet_move(dart_packet_t* dst, dart_packet_t* src);
  dart_err dart_packet_destroy(dart_packet_t* pkt);

  // generic lifecycle functions.
  dart_err dart_destroy(void* pkt);

  // error handling functions.
  char const* dart_get_error();

#ifdef __cplusplus
}
#endif

#endif
