#include <stdint.h>
#include <stdbool.h>
#include <ring_buffer.h>

void ring_buffer_put(struct ring_buffer *rb, uint8_t data){

    /* Find current head index, put data at that index,
     * increment head index, if head index greater than
     * size of buffer than roll over to 0 index. */
    rb->buffer[rb->head] = data;

    rb->head++;

    if (rb->head == rb->size){
        rb->head = 0;
    }
}

uint8_t ring_buffer_get(struct ring_buffer *rb){

    /* Find current tail index, get data at that index,
     * increment tail index, if tail index greater than
     * size of buffer than roll over to 0 index. */
    uint8_t temp = rb->buffer[rb->tail];

    rb->tail++;

    if (rb->tail == rb->size){
        rb->tail = 0;
    }

    return temp;
}

uint8_t ring_buffer_peek(const struct ring_buffer *rb){

    /* Find current tail index, get data at that index
     * without incrementing. */
    uint8_t temp = rb->buffer[rb->tail];

    return temp;
}

bool ring_buffer_full(const struct ring_buffer *rb){

    /* Ring buffer full if head index + 1 is equal to
     * tail index. */
    uint8_t index_after_head = rb->head+1;

    if (index_after_head == rb->size){
        index_after_head = 0;
    }

    return index_after_head == rb->tail;
}
bool ring_buffer_empty(const struct ring_buffer *rb){

    /* Ring buffer empty if head index is equal to
     * tail index. */
    return rb->head == rb->tail;
}
