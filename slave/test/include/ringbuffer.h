// AVR Multi Motor Control
/*! \file master/include/ringbuffer.h
 * Ring Buffer data structure
 *
 * \author Paolo Lucchesi
 */
#ifndef __RINGBUFFER_STRUCT_H
#define __RINGBUFFER_STRUCT_H
#include <stdint.h>

//! Ringbuffer fullness state
enum RINGBUFFER_STATE_E { RINGBUFFER_NOT_FULL = 0, RINGBUFFER_FULL };

//! Data type for all the ringbuffer metadata
typedef struct _ringbuffer_s {
  uint8_t *base;
  uint8_t first;
  uint8_t last;
  uint8_t size;
  uint8_t full;
} ringbuffer_t;

/*!
 * Initialize a ring buffer
 *
 * @param rb  Pointer to a preallocated ringbuffer data structure
 * @param buf Pointer to the buffer to store the ringbuffer items
 * @param buf_size size of 'buf'
 * @return 0 on success, 1 otherwise
 */
uint8_t ringbuffer_new(ringbuffer_t *rb, uint8_t *buf, uint8_t buf_size);

/*!
 * Get the dimension of the buffer
 *
 * @param rb Pointer to the ringbuffer
 * @return The size of the ringbuffer
 */
uint8_t ringbuffer_size(ringbuffer_t *rb);

/*!
 * Get the number of present items
 *
 * @param rb Pointer to the ringbuffer
 * @return The size of the ringbuffer
 */
uint8_t ringbuffer_used(ringbuffer_t *rb);

/*!
 * Is the ringbuffer empty?
 *
 * @param rb Pointer to the ringbuffer
 * @return 1 if the ringbuffer is empty or does not exist, 0 if not
 */
uint8_t ringbuffer_isempty(ringbuffer_t *rb);

/*!
 * Is the ringbuffer full?
 *
 * @param rb Pointer to the ringbuffer
 * @return 1 if the ringbuffer is full, 0 if not or not existing
 */
uint8_t ringbuffer_isfull(ringbuffer_t *rb);

/*!
 * Pop an element
 *
 * @param rb Pointer to the ringbuffer
 * @param dest Pointer to a memory area to store the popped element
 * @return 0 on success, 1 otherwise
 */
uint8_t ringbuffer_pop(ringbuffer_t *rb, uint8_t *dest);

/*!
 * Push an element
 *
 * @param rb Pointer to the ringbuffer
 * @param val Value for the item to be pushed in the ringbuffer
 * @return 0 on success, 1 otherwise
 */
uint8_t ringbuffer_push(ringbuffer_t *rb, uint8_t val);

/*!
 * Flush (i.e. reset, empty) a ringbuffer
 *
 * @param rb Pointer to the ringbuffer
 */
void ringbuffer_flush(ringbuffer_t *rb);

#endif  // __RINGBUFFER_STRUCT_H
