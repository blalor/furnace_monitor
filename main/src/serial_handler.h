#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

/*
Processes and (eventually) generates messages of the form
    0xff 0x55 <length> <data1> … <dataN> <checksum>

This is modeled on (cloned from) the iPod serial protocol.

There are deficiencies in the protocol: see sad-path tests.
*/

#include <stdint.h>

void serial_handler_init(
    void (*received_msg_handler)(const uint8_t *, const uint8_t),
    uint8_t *buf,
    uint8_t buf_len
);

void serial_handler_consume(const uint8_t);

#endif
