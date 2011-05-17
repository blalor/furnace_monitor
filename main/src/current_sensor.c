#include "current_sensor.h"

#include <avr/io.h>

void current_sensor_init() {
    DDRB &= ~_BV(PB3);
}

bool is_current_flowing() {
    // MID400 goes low when current is flowing, indicating that this device is
    // the one calling for heat (closing the circuit)
    return ((PORTB & _BV(PB3)) == 0);
}
