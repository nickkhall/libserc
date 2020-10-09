#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include "headers/serialize.h"

/*
 * ------------------------------------------------------
 * function: serlib_init_buffer
 * ------------------------------------------------------
 * params  : b - ser_buff_t**
 * ------------------------------------------------------
 * Initializes the serialized buffer type.
 * ------------------------------------------------------
 */
void serlib_init_buffer(ser_buff_t* b) {
  // create memory for serialized buffer type
  b = (ser_buff_t*) malloc(sizeof(ser_buff_t));
  if (!b) {
    printf("ERROR:: serlib - Failed to allocate memory for ser buffer in serlib_init_buffer\n");
    // @TODO : create clean up function for app, memory, etc.
    exit(1);
  }

  // create memory for serialized buffer's buffer
  b->buffer = malloc(SERIALIZE_BUFFER_DEFAULT_SIZE);
  if (!b->buffer) {
    printf("ERROR:: serlib - Failed to allocate memory for ser buffer's buffer in serlib_init_buffer\n");
    // @TODO : create clean up function for app, memory, etc.
    exit(1);
  }

  // set buffer size to default size
  b->size = SERIALIZE_BUFFER_DEFAULT_SIZE;

  // set buffer's next segment
  b->next = 0;
};

/*
 * ------------------------------------------------------
 * function: serlib_init_buffer_of_size
 * ------------------------------------------------------
 * params  : b - ser_buff_t**
 * ------------------------------------------------------
 * Initializes the serialized buffer type.
 * ------------------------------------------------------
 */
void serlib_init_buffer_of_size(ser_buff_t** b, int size) {
  if (!size) {
    printf("ERROR:: serlib - No size for serialized buffer in serlib_init_buffer_of_size\n");
    free(*b);
    exit(1);
  }
  (*b) = (ser_buff_t*) malloc(sizeof(ser_buff_t));
  if (!(*b)) {
    printf("ERROR:: serlib - Failed to allocate memory for ser buffer in serlib_init_buffer_of_size\n");
    // @TODO : create clean up function for app, memory, etc.
    exit(1);
  }

  (*b)->buffer = malloc(size);
  if (!(*b)->buffer) {
    printf("ERROR:: serlib - Failed to allocate memory for ser buffer's buffer in serlib_init_buffer_of_size\n");
    // @TODO : create clean up function for app, memory, etc.
    exit(1);
  }
  (*b)->size = size;
  (*b)->next = 0;
};

/*
 * ------------------------------------------------------
 * function: serlib_header_get_size
 * ------------------------------------------------------
 * Returns size of serialized header.
 * ------------------------------------------------------
 */
unsigned int serlib_header_get_size(void) {
  ser_header_t ser_header;
  return (sizeof(ser_header.tid)
         + sizeof(ser_header.rpc_proc_id)
         + sizeof(ser_header.rpc_call_id)
         + sizeof(ser_header.payload_size));
};

/*
 * ------------------------------------------------------
 * function: serlib_header_init
 * ------------------------------------------------------
 * Returns size of serialized header.
 * ------------------------------------------------------
 */
ser_header_t* serlib_header_init(int tid, int rpc_proc_id, int rpc_call_id, int payload_size) {
  ser_header_t* ser_header = (ser_header_t*) malloc(sizeof(ser_header_t));
  if (!ser_header) {
    printf("ERROR:: serlib - Failed to allocate memory for serialized header\n");
    exit(1);
  }

  ser_header->tid = tid;
  ser_header->rpc_proc_id = rpc_proc_id;
  ser_header->rpc_call_id = rpc_call_id;
  ser_header->payload_size = payload_size;

  return ser_header;
};

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
void serlib_buffer_skip(ser_buff_t* b, int skip_size) {
  // if the skip_size is above 0,
  // and the buffer has access to the needed memory
  if (b->next + skip_size >= 0 &&
      b->next + skip_size < b->size
  ) {
    // skip the buffer
    // (adjust the next pointer)
    b->next += skip_size;
  }
};

/*
 * ---------------------------------------------------
 * function: serlib_reset_buffer
 * ---------------------------------------------------
 * params  : b - ser_buff_t*
 * ---------------------------------------------------
 * Resets a buffer. (Sets ->next to 0)
 * ---------------------------------------------------
 */
void serlib_reset_buffer(ser_buff_t* b) {
  b->next = 0;
};

/*
 * ----------------------------------------------------
 * function: serlib_get_buffer_length
 * ----------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------
 * Gets length of serialized buffer.
 * ----------------------------------------------------
 */
int serlib_get_buffer_length(ser_buff_t* b) {
  return b->size;
};

/*
 * -------------------------------------------------------
 * function: serlib_get_buffer_data_size
 * -------------------------------------------------------
 * params  : b - ser_buff_t*
 * -------------------------------------------------------
 * Gets data size of serialized buffer.
 * -------------------------------------------------------
 */
int serlib_get_buffer_data_size(ser_buff_t* b) {
  return b->next;
};

/*
 * -----------------------------------------------------
 * function: serlib_copy_in_buffer_by_offset
 * -----------------------------------------------------
 * params  : b - ser_buff_t*
 * -----------------------------------------------------
 * 
 * -----------------------------------------------------
 */
void serlib_copy_in_buffer_by_offset(ser_buff_t* client_send_ser_buffer, int size, char* value, int offset) {
  if (offset > client_send_ser_buffer->size) {
    printf("%s(): ERROR:: serlib - Attempted to write outside of buffer limits\n", __FUNCTION__);
    return;
  }

  memcpy(client_send_ser_buffer->buffer + offset, value, size);
};

/*
 * --------------------------------------------
 * function: serlib_free_buffer
 * --------------------------------------------
 * params  : b - ser_buff_t*
 * --------------------------------------------
 * Frees the memory and destroys a buffer type.
 * --------------------------------------------
 */
void serlib_free_buffer(ser_buff_t* b) {
  free(b->buffer);
  free(b);
};

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
void serlib_serialize_data(ser_buff_t* b, char* data, int nbytes) {
  if (b == NULL) assert(0);

  ser_buff_t* buff = (ser_buff_t*)(b);
  // get total available size of buffer
  int available_size = buff->size - buff->next;
  // resize flag used for resizing buffer
  int should_resize = 0;

  // if we don't have enough memory for data in buffer
  while(available_size < nbytes) {
    // increase (multiply) buffer size by 2
    buff->size = buff->size * 2;

    // update total available size
    available_size = buff->size - buff->next;

    // set should resize flag
    should_resize = 1;
  }

  // else we have enough memory for data in buffer
  if (should_resize == 0) {
    // copy data from src to buffer's buffer (b->buffer)
    memcpy((char*)buff->buffer + buff->next, data, nbytes);

    // increase the buffers next memory to nbytes
    buff->next += nbytes;

    return;
  }

  // resize the buffer
  buff->buffer = realloc(buff->buffer, buff->size);

  // copy data to buffer's buffer (b->buffer)
  memcpy((char*)buff->buffer + buff->next, data, nbytes);

  // increase buffer's next memory by nbtyes
  buff->next += nbytes;

  return;
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_data_int_ptr
 * ----------------------------------------------------------------------
 * params  :
 *         > b      - ser_buff_t*
 *         > data   - int*
 *         > nbytes - int
 * ----------------------------------------------------------------------
 * Deserializes a buffers' string buffer for an integer.
 * ----------------------------------------------------------------------
 */
void serlib_serialize_data_int_ptr(ser_buff_t* b, int* data, int nbytes) {
  if (b == NULL) assert(0);

  ser_buff_t* buff = (ser_buff_t*)(b);
  // get total available size of buffer
  int available_size = buff->size - buff->next;
  // resize flag used for resizing buffer
  int should_resize = 0;

  // if we don't have enough memory for data in buffer
  while(available_size < nbytes) {
    // increase (multiply) buffer size by 2
    buff->size = buff->size * 2;

    // update total available size
    available_size = buff->size - buff->next;

    // set should resize flag
    should_resize = 1;
  }

  // else we have enough memory for data in buffer
  if (should_resize == 0) {
    // copy data from src to buffer's buffer (b->buffer)
    memcpy((int*)buff->buffer + buff->next, data, nbytes);

    // increase the buffers next memory to nbytes
    buff->next += nbytes;

    return;
  }

  // resize the buffer
  buff->buffer = realloc(buff->buffer, buff->size);

  // copy data to buffer's buffer (b->buffer)
  memcpy((int*)buff->buffer + buff->next, data, nbytes);

  // increase buffer's next memory by nbtyes
  buff->next += nbytes;

  return;
}

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
void serlib_deserialize_data_int(ser_buff_t* b, int dest, int size) {
  if (!b || !b->buffer) assert(0);
  if (!size) return;
  if ((b->size - b->next) < size) assert(0);

  // copy data from dest to string buffer
  dest = (int)*(b->buffer + b->next);

  // increment the buffer's next pointer
  b->next += size;
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_deserialize_data_int_ptr
 * ----------------------------------------------------------------------
 * params  :
 *         > b    - ser_buff_t*
 *         > dest - int*
 *         > size - int
 * ----------------------------------------------------------------------
 * Deserializes a buffers' string buffer for an integer.
 * ----------------------------------------------------------------------
 */
void serlib_deserialize_data_int_ptr(ser_buff_t* b, int* dest, int size) {
  if (!b || !b->buffer) assert(0);
  if (!size) return;
  if ((b->size - b->next) < size) assert(0);

  // copy data from dest to string buffer
  memcpy(dest, (int*)(b->buffer + b->next), size);

  // increment the buffer's next pointer
  b->next += size;
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_time_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Serializes a buffers' employee_t buffer.
 * ----------------------------------------------------------------------
 */
void serlib_serialize_time_t(ser_buff_t* b, time_t dest, int size) {
  if (b == NULL) assert(0);

  ser_buff_t* buff = (ser_buff_t*)(b);
  // get total available size of buffer
  int available_size = buff->size - buff->next;
  // resize flag used for resizing buffer
  int should_resize = 0;

  // if we don't have enough memory for data in buffer
  while(available_size < nbytes) {
    // increase (multiply) buffer size by 2
    buff->size = buff->size * 2;

    // update total available size
    available_size = buff->size - buff->next;

    // set should resize flag
    should_resize = 1;
  }

  // else we have enough memory for data in buffer
  if (should_resize == 0) {
    // copy data from src to buffer's buffer (b->buffer)
    memcpy((time_t*)buff->buffer + buff->next, data, nbytes);

    // increase the buffers next memory to nbytes
    buff->next += nbytes;

    return;
  }

  // resize the buffer
  buff->buffer = realloc(buff->buffer, buff->size);

  // copy data to buffer's buffer (b->buffer)
  memcpy((time_t*)buff->buffer + buff->next, data, nbytes);

  // increase buffer's next memory by nbtyes
  buff->next += nbytes;

  return;
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_time_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Serializes a buffers' employee_t buffer.
 * ----------------------------------------------------------------------
 */
void serlib_deserialize_time_t(ser_buff_t*b, time_t* dest, int size) {
  if (!b || !b->buffer) assert(0);
  if (!size) return;
  if ((b->size - b->next) < size) assert(0);

  // copy data from dest to string buffer
  memcpy(dest, (b->buffer + b->next), size);

  // increment the buffer's next pointer
  b->next += size;
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_deserialize_data
 * ----------------------------------------------------------------------
 * params  :
 *         > dest - char*
 *         > b    - ser_buff_t*
 *         > size - int
 * ----------------------------------------------------------------------
 * Deserializes a buffers' string buffer.
 * ----------------------------------------------------------------------
 */
void serlib_deserialize_data(ser_buff_t* b, char* dest, int size) {
  if (!b || !b->buffer) assert(0);
  if (!size) return;
  if ((b->size - b->next) < size) assert(0);

  // copy data from dest to string buffer
  memcpy(dest, b->buffer + b->next, size);

  // increment the buffer's next pointer
  b->next += size;
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_list_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Deserializes a buffers' employee_t buffer.
 * ----------------------------------------------------------------------
 */
void serlib_serialize_list_t(list_t* list,
                             ser_buff_t* b,
                             void (* serialize_fn_ptr)(void *, ser_buff_t*))
{
  // if this is a sentinel section, return null
  if (!list) {
    unsigned int sentinel = 0xFFFFFFFF;
    serlib_serialize_data(b, (char*)&sentinel, sizeof(unsigned int));
    return;
  }

  serlib_serialize_list_node_t(list->head, b, serialize_fn_ptr);
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_serialize_list_node_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Serializes a employee list node.
 * ----------------------------------------------------------------------
 */
void serlib_serialize_list_node_t(list_node_t* list_node, ser_buff_t* b, void (*serialize_fn_ptr)(void*, ser_buff_t*))
{
  // if this is a sentinel section, return null
  if (!list_node) {
    unsigned int sentinel = 0xFFFFFFFF;
    serlib_serialize_data(b, (char*)&sentinel, sizeof(unsigned int));
    return;
  }

  serialize_fn_ptr(list_node->data, b);
  serlib_serialize_list_node_t(list_node->next, b, serialize_fn_ptr);
};

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
list_t* serlib_deserialize_list_t(ser_buff_t* b, void (*deserialize_fn_ptr)(void*, ser_buff_t*)) {
  if (!b || !b->buffer) return NULL;

  // create new generic linked list memory
  list_t* list = malloc(sizeof(list_t));
  serlib_list_new(list, sizeof(list_t), NULL);

  // set linked list head and deserialize data
  serlib_deserialize_list_node_t(list->head, b, deserialize_fn_ptr);
  list->tail = NULL;

  return list;
};

/*
 * ----------------------------------------------------------------------
 * function: serlib_deserialize_list_node_t
 * ----------------------------------------------------------------------
 * params  : b - ser_buff_t*
 * ----------------------------------------------------------------------
 * Deserializes a employee list node.
 * ----------------------------------------------------------------------
 */
void serlib_deserialize_list_node_t(list_node_t* list_node, ser_buff_t* b, void (*deserialize_fn_ptr)(void*, ser_buff_t*)) {
  int sentinel = 0xFFFFFFFF;
  serlib_deserialize_data(b, (char*)&sentinel, sizeof(int));
  if (sentinel == 0xFFFFFFFF) {
    return;
  }

  serlib_buffer_skip(b, (int)(-1 * sizeof(int)));

  deserialize_fn_ptr(list_node->data, b);
  serlib_deserialize_list_node_t(list_node->next, b, deserialize_fn_ptr);
};

/*
 * ------------------------------------------------------
 * function: serlib_list_new
 * ------------------------------------------------------
 * params:
 *       > list      - list_t*
 *       > elem_size - int
 *       > freeFn    - function pointer
 *          > params: void*
 * ------------------------------------------------------
 * Creates a new linked list.
 * ------------------------------------------------------
 */
void serlib_list_new(list_t* list, int elem_size, void (*freeFn)(void *)) {
  // make sure there is a size
  assert(elem_size > 0);
  
  // set the default values
  list->logical_length = 0;
  list->elem_size = elem_size;
  list->head = serlib_list_new_node(elem_size);
  list->tail= list->head;

  // pass freeing function ptr
  list->freeFn = freeFn;
};

/*
 * ------------------------------------------------------
 * function: serlib_list_new_node
 * ------------------------------------------------------
 * params  : size - int
 * ------------------------------------------------------
 * Creates a new linked list node.
 * ------------------------------------------------------
 */
list_node_t* serlib_list_new_node(int size) {
  list_node_t* list_node = (list_node_t*) malloc(sizeof(list_node_t));

  list_node->data = (void*) malloc(sizeof(size));
  list_node->next = NULL;

  return list_node;
};

/*
 * ------------------------------------------------------
 * function: serlib_list_destroy
 * ------------------------------------------------------
 * params  : list - list_t*
 * ------------------------------------------------------
 * Destroys a linked list.
 * ------------------------------------------------------
 */
void serlib_list_destroy(list_t* list) {
  // create current node pointer
  list_node_t* current_node;

  while(list->head != NULL) {
    // set current node to list head to start
    current_node = list->head;

    // call clean up pointer function if there is one
    if (list->freeFn) {
      list->freeFn(current_node->data);
    }

    // free up node's data
    free(current_node->data);
    // free up node's pointer
    free(current_node);
  }
};

/*
 * ------------------------------------------------------
 * function: serlib_list_prepend
 * ------------------------------------------------------
 * params  :
 *         > list    - list_t*
 *         > element - void*
 * ------------------------------------------------------
 * Prepends a node on a linked list.
 * ------------------------------------------------------
 */
void serlib_list_prepend(list_t* list, void* element) {
  // allocate memory for node
  list_node_t* node = malloc(sizeof(list_node_t));

  // create memory for node data
  node->data = malloc(list->elem_size);
  // copy data to node
  memcpy(node->data, element, list->elem_size);

  // set the next of node to list head
  node->next = list->head;
  // set list head to current node
  list->head = node;

  // if first node
  if (!list->tail) {
    // set list / node tail to head
    list->tail = list->head;
  }

  // increment count of nodes
  list->logical_length++;
};

/*
 * ------------------------------------------------------
 * function: serlib_list_append
 * ------------------------------------------------------
 * params  :
 *         > list    - list_t*
 *         > element - void*
 * ------------------------------------------------------
 * Appends a node to a linked list.
 * ------------------------------------------------------
 */
void serlib_list_append(list_t* list, void* element) {
  // create memory for node
  list_node_t* node = malloc(sizeof(list_node_t));
  // create memory for node data
  node->data = malloc(list->elem_size);
  // set node pointer to NULL
  node->next = NULL;

  // copy the data into the node
  memcpy(node->data, element, list->elem_size);

  // if this is first node in list
  if (list->logical_length == 0) {
    list->head = node;
    list->tail = node;
  // else, we have other nodes, so append
  } else {
    list->tail->next = node;
    list->tail = node;
  }

  // increment count of nodes
  list->logical_length++;
};

/*
 * ------------------------------------------------------
 * function: serlib_list_get_size
 * ------------------------------------------------------
 * params  : list - list_t*
 * ------------------------------------------------------
 * Returns size of linked list.
 * ------------------------------------------------------
 */
int serlib_list_get_size(list_t* list) {
  return list->logical_length;
};

/*
 * ------------------------------------------------------
 * function: serlib_list_iterate
 * ------------------------------------------------------
 * params  :
 *         > list          - list_t*
 *         > list_iterator - function pointer
 *           > params: void*
 * ------------------------------------------------------
 * Iterates through a linked list.
 * ------------------------------------------------------
 */
void serlib_list_iterate(list_t* list, bool (*list_iterator)(void *)) {
  assert(list_iterator != NULL);

  list_node_t* node = list->head;
  bool result = true;
  while(node != NULL && result) {
    result = list_iterator(node->data);
    node = node->next;
  }
};

/*
 * ------------------------------------------------------
 *
 * ------------------------------------------------------
 *
 * ------------------------------------------------------
 *
 * ------------------------------------------------------
 */
void serlib_list_get_head(list_t* list, void* element, bool should_remove) {
};

/*
 * ------------------------------------------------------
 *
 * ------------------------------------------------------
 *
 * ------------------------------------------------------
 *
 * ------------------------------------------------------
 */
void serlib_list_get_tail(list_t* list, void* element) {
};

