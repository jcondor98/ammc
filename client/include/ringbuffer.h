// AVR Multi Motor Control
/*! \file client/include/ringbuffer.h
 * Multi-threaded, lock-free ringbuffer data structure
 *
 * \author Paolo Lucchesi
 */
#ifndef __RINGBUFFER_STRUCT_H
#define __RINGBUFFER_STRUCT_H
#include <stddef.h>
#include <pthread.h>

//! Ringbuffer fullness state
enum RINGBUFFER_STATE_E { RINGBUFFER_NOT_FULL = 0, RINGBUFFER_FULL };

//! Type definition for a ringbuffer
typedef struct _ringbuffer_s {
  unsigned char *base;
  pthread_mutex_t lock[1];
  size_t first;
  size_t last;
  size_t size;
  unsigned char full;
} ringbuffer_t;

/*!
 * Initialize a ring buffer
 *
 * @param buf_size Size of the ringbuffer in bytes (each item is a byte)
 * @returns A pointer to the created ringbuffer on success, NULL on failure
 */
ringbuffer_t *ringbuffer_new(size_t buf_size);

/*!
 * Delete a ringbuffer
 *
 * @param rb Pointer to the ringbuffer to destroy
 */
void ringbuffer_delete(ringbuffer_t *rb);

/*!
 * Get the maximum number of items
 *
 * @param rb Pointer to the ringbuffer
 * @returns The maximum number of items
 */
size_t ringbuffer_size(ringbuffer_t *rb);

/*!
 * Get the number of present items
 *
 * @param rb Pointer to the ringbuffer
 * @returns The number of present items
 */
size_t ringbuffer_used(ringbuffer_t *rb);

/*!
 * Is a ringbuffer empty?
 *
 * @param rb Pointer to the ringbuffer
 * @returns 1 if the buffer is empty, 0 if not or if it does not exist
 */
unsigned char ringbuffer_isempty(ringbuffer_t *rb);

/*!
 * Is a ringbuffer full?
 *
 * @param rb Pointer to the ringbuffer
 * @returns 1 if the buffer is full or if it does not exist, 0 if not
 */
unsigned char ringbuffer_isfull(ringbuffer_t *rb);

/*!
 * Pop an element
 *
 * @param rb   Pointer to the ringbuffer
 * @param dest Buffer to store the popped element
 * @returns 0 on success, 1 otherwise
 */
unsigned char ringbuffer_pop(ringbuffer_t *rb, unsigned char *dest);

/*!
 * Push an element
 *
 * @param rb  Pointer to the ringbuffer
 * @param val Value of the item to push
 * @returns 0 on success, 1 otherwise
 */
unsigned char ringbuffer_push(ringbuffer_t *rb, unsigned char val);

/*!
 * Flush (i.e. reset, empty) a ringbuffer
 *
 * @param rb Pointer to the ringbuffer to flush
 */
void ringbuffer_flush(ringbuffer_t *rb);

#if defined(TEST)
/*!
 * Print the internal elements (without the raw buffer) of a ringbuffer
 *
 * @param rb Pointer to the ringbuffer to print
 */
void ringbuffer_print(ringbuffer_t *rb);
#endif

#endif    // __RINGBUFFER_STRUCT_H
