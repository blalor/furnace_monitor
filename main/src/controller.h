#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// safety/sanity restriction; don't let the timer be set for more than 15 
// minutes UINT16_MAX (?) is something like 18 HOURS!
#define TIMER_MAX (15 * 60)

typedef enum __zone_state {
    ZONE_STATE_UNKNOWN,
    ZONE_STATE_ACTIVE,
    ZONE_STATE_INACTIVE
} ZoneState;

typedef struct __furnace_status {
    ZoneState zone_state;
    bool furnace_powered;
    uint16_t timer_remaining;
} FurnaceStatus;

typedef struct __furnace_start_timer {
    char literal_S;
    uint16_t duration_seconds;
} FurnaceStartTimerCommand;

void furnace_controller_init(
    void (*msg_sender)(const void *data, const size_t data_len)
);
    
FurnaceStatus furnace_get_status(void);

void furnace_timer_start(const uint16_t duration_seconds);
void furnace_timer_cancel(void);
void furnace_update_timer(void);

void furnace_handle_incoming_msg(const void *msg_ptr, const size_t len);
void furnace_send_status(void);
#endif
