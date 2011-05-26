#ifndef FURNACE_POWER_SENSOR_H
#define FURNACE_POWER_SENSOR_H

#include <stdbool.h>

/*
 * initializes furnace power sensor.
 */
void furnace_power_sensor_init(void);

/*
 * returns true if power to furnace is on, regardless of if thermostat is calling 
 * for heat currently.
 */
bool is_furnace_power_on(void);

#endif
