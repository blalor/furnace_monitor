#ifndef ZONE_MONITOR_H
#define ZONE_MONITOR_H

#include <stdbool.h>

/*
 * initializes zone monitor.
 */
void zone_monitor_init(void);

/*
 * returns true if zone is calling for heat.
 */
bool is_zone_active(void);

#endif
