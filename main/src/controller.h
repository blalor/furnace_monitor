#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct __furnace_status {
    bool zone_active;
    uint16_t timer_remaining;
} FurnaceStatus;

typedef struct __furnace_start_timer {
    char literal_S;
    uint8_t duration_seconds;
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
