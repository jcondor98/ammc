// AVR Multi Motor Control
/*! \file master/source/ringbuffer.c
 * Ring Buffer data structure
 *
 * \author Paolo Lucchesi
 */
#include "ringbuffer.h"


// [AUX] Get the real index of an item given its virtual address
static inline uint8_t real_idx(const ringbuffer_t *rb, uint8_t virt) {
  return (virt + rb->first) % rb->size;
}

// [AUX] Get the virtual index of an item given its real address
static inline uint8_t virt_idx(const ringbuffer_t *rb, uint8_t real) {
  return (real + rb->size - rb->first) % rb->size;
}

// [AUX] Get the shifted (i.e. incremented) index for a virtual one
static inline uint8_t shifted_idx(const ringbuffer_t *rb, uint8_t idx) {
  return (idx + 1) % rb->size;
}


// Initialize a ring buffer
uint8_t ringbuffer_new(ringbuffer_t *rb, uint8_t *buf, uint8_t buf_size) {
  if (!buf || buf_size < 2) return 1;
  *rb = (ringbuffer_t) { buf, 0, 0, buf_size, RINGBUFFER_NOT_FULL };
  return 0;
}


// Get the dimension of the buffer
uint8_t ringbuffer_size(ringbuffer_t *rb) { return rb ? rb->size : 0; }

// Get the number of present items
uint8_t ringbuffer_used(ringbuffer_t *rb) {
  if (!rb) return 0;
  if (rb->full) return rb->size;
  return virt_idx(rb, rb->last);
}

// Is the ringbuffer empty?
uint8_t ringbuffer_isempty(ringbuffer_t *rb) {
  return (!rb || (!rb->full && rb->first == rb->last)) ? 1 : 0;
}

// Is the ringbuffer full?
uint8_t ringbuffer_isfull(ringbuffer_t *rb) {
  return (rb && rb->full && rb->first == rb->last) ? 1 : 0;
}

// Pop an element
uint8_t ringbuffer_pop(ringbuffer_t *rb, uint8_t *dest) {
  if (!rb || ringbuffer_isempty(rb))
    return 1;
  *dest = rb->base[rb->first];
  rb->first = shifted_idx(rb, rb->first);
  rb->full = RINGBUFFER_NOT_FULL; // Less expensive than check-and-set
  return 0;
}

// Push an element
uint8_t ringbuffer_push(ringbuffer_t *rb, uint8_t val) {
  if (!rb || rb->full)
    return 1;
  rb->base[rb->last] = val;
  rb->last = shifted_idx(rb, rb->last);
  if (rb->first == rb->last)
    rb->full = RINGBUFFER_FULL;
  return 0;
}

// Flush (i.e. reset, empty) a ringbuffer
void ringbuffer_flush(ringbuffer_t *rb) {
  if (!rb) return;
  rb->first = 0;
  rb->last = 0;
  rb->full = 0;
}
