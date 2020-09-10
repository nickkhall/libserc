#ifndef __SERLIB_SERIALIZE_H__
#define __SERLIB_SERIALIZE_H__

#define SERIALIZE_BUFFER_DEFAULT_SIZE 100

#include <time.h>

#include "common.h"

typedef struct _ser_buff_t {
  char* buffer;
  int size;
  int next;
} ser_buff_t;

typedef struct _list_node_t {
  void* data;
  struct _list_node_t* next;
} list_node_t;

typedef struct _list_t {
  int logical_length;
  int elem_size;
  list_node_t* head;
  list_node_t* tail;
  void (*freeFn) (void*);
} list_t;

typedef struct _ser_header_t {
  unsigned int tid;
  unsigned int rpc_proc_id;
  unsigned int rpc_call_id;
  unsigned int payload_size;
} ser_header_t;

typedef struct _client_param_t {
  unsigned int recv_buff_size;
  ser_buff_t*  recv_ser_b;
} client_param_t;

/*
 * ------------------------------------------------------
 * function: serlib_init_buffer
 * ------------------------------------------------------
 * params  : b - ser_buff_t**
 * ------------------------------------------------------
 * Initializes the serialized buffer type.
 * ------------------------------------------------------
 */
void serlib_init_buffer(ser_buff_t* b);

/*
 * ------------------------------------------------------
 * function: serlib_init_buffer_of_size
 * ------------------------------------------------------
 * params  : b - ser_buff_t**
 * ------------------------------------------------------
 * Initializes the serialized buffer type.
 * ------------------------------------------------------
 */
void serlib_init_buffer_of_size(ser_buff_t** b, int size);

/*
 * ------------------------------------------------------
 * function: serlib_header_get_size
 * ------------------------------------------------------
 * Returns size of serialized header.
 * ------------------------------------------------------
 */
unsigned int serlib_header_get_size(void);

/*
 * ------------------------------------------------------
 * function: serlib_header_init
 * ------------------------------------------------------
 * Returns size of serialized header.
 * ------------------------------------------------------
 */
ser_header_t* serlib_header_init(int tid, int rpc_proc_id, int rpc_call_id, int payload_size);

/*
 * --------------------------------------------------------------------
 * function: serlib_buffer_skip
 * --------------------------------------------------------------------
 * params  :
 *         > buffer    - ser_buff_t*
 *         > skip_size - int
 * --------------------------------------------------------------------
 * Skips a section of memory in the buffer's buffer. 
 * (In/Decrements the next pointer)
 * --------------------------------------------------------------------
 */
void serlib_buffer_skip(ser_buff_t* b, unsigned long int skip_size);

/*
 * ---------------------------------------------------
 * function: serlib_reset_buffer
 * ---------------------------------------------------
 * params  : b - ser_buff_t*
 * ---------------------------------------------------
 * Resets a buffer. (Sets ->next to 0)
 * ---------------------------------------------------
 */
void serlib_reset_buffer(ser_buff_t* b);

/*
 * ---------------------------------------------------
 * function: serlib_get_buffer_length
 * ---------------------------------------------------
 * params  : b - ser_buff_t*
 * ---------------------------------------------------
 * Frees the memory and destroys a buffer type.
 * ---------------------------------------------------
 */
int serlib_get_buffer_length(ser_buff_t* b);

/*
 * -----------------------------------------------------
 * function: serlib_get_buffer_data_size
 * -----------------------------------------------------
 * params  : b - ser_buff_t*
 * -----------------------------------------------------
 * Gets data size of serialized buffer.
 * -----------------------------------------------------
 */
int serlib_get_buffer_data_size(ser_buff_t* b);

/*
 * -----------------------------------------------------
 * function: serlib_copy_in_buffer_by_offset
 * -----------------------------------------------------
 * params  : b - ser_buff_t*
 * -----------------------------------------------------
 * 
 * -----------------------------------------------------
 */
void serlib_copy_in_buffer_by_offset(ser_buff_t* client_send_ser_buffer, int size, char* value, int offset);

/*
 * --------------------------------------------
 * function: serlib_free_buffer
 * --------------------------------------------
 * params  : b - ser_buff_t*
 * --------------------------------------------
 * Frees the memory and destroys a buffer type.
 * --------------------------------------------
 */
void serlib_free_buffer(ser_buff_t* b);

/*
 * ------------------------------------------------------------------------
 * function: serlib_serialize_data
 * ------------------------------------------------------------------------
 * params  : 
 *           > b      - ser_buff_t**
 *           > data   - char*
 *           > nbytes - int
 * ------------------------------------------------------------------------
 * Serializes string data to a given valid serialized string buffer.
 * ------------------------------------------------------------------------
 */
void serlib_serialize_data(ser_buff_t* b, char* data, int nbytes);

/*
 * ----------------------------------------------------------------------
 * function: serlib_deserialize_data
 * ----------------------------------------------------------------------
 * params  :
 *         > b    - ser_buff_t*
 *         > dest - char*
 *         > size - int
 * ----------------------------------------------------------------------
 * Deserializes a buffers' string buffer.
 * ----------------------------------------------------------------------
 */
void serlib_deserialize_data(ser_buff_t* b, char* dest, int size);

/*
 * ----------------------------------------------------------------------
 * function: serlib_deserialize_data_int
 * ----------------------------------------------------------------------
 * params  :
 *         > dest - int*
 *         > b    - ser_buff_t*
 *         > size - int
 * ----------------------------------------------------------------------
 * Deserializes a buffers' string buffer for an integer.
 * ----------------------------------------------------------------------
 */
void serlib_deserialize_data_int(ser_buff_t* b, int* dest, int size);

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_data_time_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Serializes a time_t.
 * ----------------------------------------------------------------------
 */
void serlib_serialize_data_time_t(time_t* dest, ser_buff_t*b, int size);

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_data_time_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Deserializes a time_t.
 * ----------------------------------------------------------------------
 */
void serlib_deserialize_data_time_t(time_t* dest, ser_buff_t*b, int size);


/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_list_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Serializes a list.
 * ----------------------------------------------------------------------
 */
void serlib_serialize_list_t(list_t* list,
                             ser_buff_t* b,
                             void (* serialize_fn_ptr)(void *, ser_buff_t*));

/*
 * ------------------------------------------------------------------------------
 * function: serlib_deserialize_list_t
 * ------------------------------------------------------------------------------
 * params  :
 *         > b                - ser_buff_t*
 *         > serialize_fn_ptr - function pointer to function (void*, ser_buff_t*)
 * ------------------------------------------------------------------------------
 * Deserializes a employee list.
 * ------------------------------------------------------------------------------
 */
void serlib_deserialize_list_t(ser_buff_t* b, void (*serialize_fn_ptr)(void *, ser_buff_t*));

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_list_node_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Serializes a list node.
 * ----------------------------------------------------------------------
 */
void serlib_serialize_list_node_t(list_node_t* list_node, ser_buff_t* b, void (*serialize_fn_ptr)(void*, ser_buff_t* b));

/*
 * ----------------------------------------------------------------------
 * function: serlib_deserialize_list_node_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Deserializes a list node.
 * ----------------------------------------------------------------------
 */
list_node_t* serlib_deserialize_list_node_t(ser_buff_t* b, void (*serialize_fn_ptr)(void *, ser_buff_t*));

#endif

