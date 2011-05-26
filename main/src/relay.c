#include "relay.h"

#include <avr/io.h>

void relay_init() {
    // set pin to output
    DDRB |= _BV(PB4);
    
    relay_off();
}

void relay_on() {
    PORTB |= _BV(PB4);
}

void relay_off() {
    PORTB &= ~_BV(PB4);
}
