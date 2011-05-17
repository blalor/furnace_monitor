#include "zone_monitor.h"

#include <avr/io.h>

void zone_monitor_init() {
    DDRB &= ~_BV(PB2);
}

bool is_zone_active() {
    return (PORTB & _BV(PB2)) == _BV(PB2);
}
