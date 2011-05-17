#include "current_sensor.h"

#include <avr/io.h>

void current_sensor_init() {
    DDRB &= ~_BV(PB3);
}

bool is_current_flowing() {
    return ((PORTB & _BV(PB3)) == 0);
}
