#ifndef SIMULATE

#ifndef __INCLUDE_RING_H__
#define __INCLUDE_RING_H__

#include <kern/lock.h>

typedef struct {
    uint16_t capacity;
    uint8_t *buf;
    uint16_t read; // next position to read from
    uint16_t write; // next position to write to
    struct lock lock;
} ring_buf;

void ring_init(ring_buf *ring, uint16_t size);
uint16_t ring_write(ring_buf *ring, uint8_t *buf, uint16_t count);
uint16_t ring_read(ring_buf *ring, uint8_t *buf, uint16_t count);
uint16_t ring_size(ring_buf *ring);

#endif

#endif
