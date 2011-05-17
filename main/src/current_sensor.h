#ifndef CURRENT_SENSOR_H
#define CURRENT_SENSOR_H

#include <stdbool.h>

/*
 * initializes current sensor.
 */
void current_sensor_init(void);

/*
 * returns true if current is flowing, indicating that this device is the one
 * calling for heat.
 */
bool is_current_flowing(void);

#endif
