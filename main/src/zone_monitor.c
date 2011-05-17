#include "zone_monitor.h"

#include <avr/io.h>

void zone_monitor_init() {
    DDRB &= ~_BV(PB2);
}

bool is_zone_active() {
    // MID400 goes high when no voltage is present
    return (PORTB & _BV(PB2)) == _BV(PB2);
}
