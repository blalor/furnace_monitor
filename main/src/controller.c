#include "controller.h"

#include "zone_monitor.h"
#include "relay.h"
#include "current_sensor.h"

static uint16_t timer_remaining; // in seconds

void furnace_controller_init() {
    furnace_timer_cancel();
}

FurnaceStatus furnace_get_status() {
    FurnaceStatus status;

    status.timer_remaining = timer_remaining;
    
    status.zone_active = false;
    
    if (timer_remaining == 0) {
        // timer should be inactive, so only the zone monitor is relevant
        status.zone_active = is_zone_active();
    } else {
        // furnace timer override active; only current sensor is relevant
        status.zone_active = is_current_flowing();
    }
    
    return status;
}

void furnace_timer_start(const uint16_t duration_seconds) {
    relay_on();
    timer_remaining = duration_seconds;
}

void furnace_timer_cancel() {
    relay_off();
    timer_remaining = 0;
}
