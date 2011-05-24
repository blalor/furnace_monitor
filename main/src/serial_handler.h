#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <stdint.h>

void serial_handler_init(
    void (*received_msg_handler)(const uint8_t *, const uint8_t),
    uint8_t *buf,
    uint8_t buf_len
);

void serial_handler_consume(const uint8_t);

#endif
