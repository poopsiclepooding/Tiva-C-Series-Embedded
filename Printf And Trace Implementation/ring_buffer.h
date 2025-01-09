#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>

struct ring_buffer{
    uint8_t *buffer; // Pointer to buffer
    uint8_t size; // Size of buffer
    uint8_t head; // Head index of buffer
    uint8_t tail; // Tail index of buffer
};

void ring_buffer_put(struct ring_buffer *rb, uint8_t data);
uint8_t ring_buffer_get(struct ring_buffer *rb);
uint8_t ring_buffer_peek(const struct ring_buffer *rb);
bool ring_buffer_full(const struct ring_buffer *rb);
bool ring_buffer_empty(const struct ring_buffer *rb);


#endif /* RING_BUFFER_H_ */
