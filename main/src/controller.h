#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __furnace_status {
    bool zone_active;
    uint16_t timer_remaining;
} FurnaceStatus;

void furnace_controller_init(void);
    
FurnaceStatus furnace_get_status(void);

void furnace_timer_start(const uint16_t duration_seconds);
void furnace_timer_cancel(void);

#endif
