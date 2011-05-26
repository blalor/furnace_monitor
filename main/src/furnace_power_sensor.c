#include "furnace_power_sensor.h"

#include <avr/io.h>

void furnace_power_sensor_init() {
    DDRB &= ~_BV(PB3);
}

bool is_furnace_power_on() {
    // MID400 goes high when furnace power is off, reads low when 
    // furnace power on
    return ((PORTB & _BV(PB3)) == 0);
}
