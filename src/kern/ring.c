#ifndef SIMULATE

#include <kern/global.h>
#include <stdlib.h>
#include <string.h>
#include <kern/thread.h>
#include <kern/lock.h>
#include <kern/ring.h>

void ring_init(ring_buf *ring, uint16_t size) {
    ring->capacity = size;
    ring->buf = (uint8_t*)malloc(size);
    ring->read = ring->capacity;
    ring->write = 0;
    init_lock(&ring->lock, "ring lock");
}

uint16_t ring_size(ring_buf *ring) {
    acquire(&ring->lock);
    uint16_t size;
    if (ring->read == ring->capacity)
        size = 0;
    else
        size = ((ring->write - ring->read + ring->capacity - 1) % ring->capacity) + 1;
    release(&ring->lock);
    return size;
}

uint16_t ring_write(ring_buf *ring, uint8_t *buf, uint16_t count) {
    acquire(&ring->lock);
    uint16_t n = ring_size(ring);

    // check if the ring is about to become non-empty
    if (n + count > 0 && ring->read == ring->capacity)
        ring->read = ring->write;

    // now work out how much we can actually write
    n = ring->capacity - n;
    if (count < n)
        n = count;
    count = n;

    // anything to write?
    if (n > 0) {
        // will we run off the end of the buffer?
        if (ring->write + n >= ring->capacity) {
            uint16_t run = ring->capacity - ring->write;
            if (buf) memcpy(ring->buf+ring->write, buf, run);
            n -= run;
            if (buf) buf += run;
            ring->write = 0;
        }
        // write any remaining bytes
        if (n > 0) {
            if (buf) memcpy(ring->buf+ring->write, buf, n);
            ring->write += n;
        }
    }
    release(&ring->lock);
    return count;
}

uint16_t ring_read(ring_buf *ring, uint8_t *buf, uint16_t count) {
    acquire(&ring->lock);
    uint16_t n = ring_size(ring);

    // will the ring be empty after reading?
    uint8_t empty = ((int16_t)n - count <= 0);

    // now work out how much we can actually read
    if (count < n)
        n = count;
    count = n;

    // anything to read?
    if (n > 0) {
        // will we run off the end of the buffer?
        if (ring->read + n >= ring->capacity) {
            uint16_t run = ring->capacity - ring->read;
            if (buf) memcpy(buf, ring->buf+ring->read, run);
            n -= run;
            if (buf) buf += run;
            ring->read = 0;
        }
        // read any remaining bytes
        if (n > 0) {
            if (buf) memcpy(buf, ring->buf+ring->read, n);
            ring->read += n;
        }
    }

    // flag empty state if it occurs
    if (empty)
        ring->read = ring->capacity;
    release(&ring->lock);
    return count;
}

#endif

