#include "controller.h"

#include "zone_monitor.h"
#include "relay.h"
#include "furnace_power_sensor.h"

#include <string.h>

static uint16_t timer_remaining; // in seconds
static void (*msg_sender)(const void *data, const size_t data_len);

void furnace_controller_init(
    void (*msg_sender_fn)(const void *data, const size_t data_len)
) {
    msg_sender = msg_sender_fn;
    furnace_timer_cancel();
}

FurnaceStatus furnace_get_status() {
    FurnaceStatus status;

    status.timer_remaining = timer_remaining;
    status.furnace_powered = is_furnace_power_on();
    
    status.zone_state = ZONE_STATE_UNKNOWN;
    
    if (status.timer_remaining == 0) {
        // timer should be inactive, so only the zone monitor is relevant
        if (is_zone_active()) {
            status.zone_state = ZONE_STATE_ACTIVE;
        } else {
            status.zone_state = ZONE_STATE_INACTIVE;
        }
    }
    
    return status;
}

void furnace_timer_start(const uint16_t duration_seconds) {
    // don't allow the timer to be started for a duration that's too long
    if (duration_seconds < TIMER_MAX) {
        relay_on();
        timer_remaining = duration_seconds;
    }
}

void furnace_timer_cancel() {
    relay_off();
    timer_remaining = 0;
}

void furnace_update_timer() {
    if (timer_remaining > 0) {
        timer_remaining -= 1;
        
        if (timer_remaining == 0) {
            furnace_timer_cancel();
        }
    }
}

void furnace_handle_incoming_msg(const void *msg_ptr, const size_t len) {
    // by convention, all commands MUST have their first byte be a char
    // this will determine the type of structure received
    const char cmd_byte = ((char *)msg_ptr)[0];
    
    if (cmd_byte == 'S') {
        FurnaceStartTimerCommand fst;
        memcpy(&fst, msg_ptr, len);
        
        furnace_timer_start(fst.duration_seconds);
    } else if (cmd_byte == 'C') {
        furnace_timer_cancel();
    }
}

void furnace_send_status() {
    FurnaceStatus status = furnace_get_status();
    msg_sender((const void *)&status, sizeof(FurnaceStatus));
}