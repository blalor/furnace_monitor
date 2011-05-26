#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

#include <stdlib.h>
#include <string.h>

extern "C" {
    #include <avr/io.h>
    
    #include "controller.h"
    #include "relay.h"
}

// spy for the furnace controller to use when sending data
// same as serial_handler_send
static void *spy_data_received;
static size_t spy_data_len_received;
static void spy_msg_sender(const void *data, const size_t data_len) {
    spy_data_len_received = data_len;
    spy_data_received = malloc(spy_data_len_received);
    memcpy(spy_data_received, data, data_len);
}

TEST_GROUP(ControllerTests) {
    void setup() {
        spy_data_received = NULL;
        spy_data_len_received = 0;
        
        furnace_controller_init(spy_msg_sender);

        virtualPORTB = B00000000; // zone monitor inactive, furnace power on
    }
    
    void teardown() {
        free(spy_data_received);
    }
};

/*
    send samples every 15 seconds
    start timer to call for heat
    cancel timer
    
    voltage sensor on PB2
    furnace power sensor on PB3
    relay on PB4
*/

/*
 * confirm proper setup after initialization
 */
TEST(ControllerTests, Initialization) {
    relay_on();
    
    furnace_controller_init(spy_msg_sender);
    
    FurnaceStatus status = furnace_get_status();
    
    // make sure relay is off
    BYTES_EQUAL(B00000000, virtualPORTB);
    LONGS_EQUAL(0, status.timer_remaining);
}

TEST(ControllerTests, CheckStatusZoneActive) {
    // timer inactive, thermostat calling for heat
    virtualPORTB = B00000100;
    
    FurnaceStatus status = furnace_get_status();
    
    CHECK_TRUE(status.zone_state == ZONE_STATE_ACTIVE);
}

TEST(ControllerTests, CheckStatusZoneInactive) {
    // timer inactive, thermostat not calling for heat
    virtualPORTB = B00000000;
    
    FurnaceStatus status = furnace_get_status();
    
    CHECK_TRUE(status.zone_state == ZONE_STATE_INACTIVE);
}

TEST(ControllerTests, TimerStartActivatesRelay) {
    virtualPORTB = 0;
    
    furnace_timer_start(120);
    
    FurnaceStatus status = furnace_get_status();
    
    BYTES_EQUAL(B00010000, virtualPORTB);
    LONGS_EQUAL(120, status.timer_remaining);
}

TEST(ControllerTests, TimerCancelDeactivatesRelay) {
    // check that furnace_timer_cancel deactivates the relay, and that a
    // subsequent furnace_get_status() shows the timer is stopped
    
    virtualPORTB = 0;
    
    furnace_timer_start(120);
    
    furnace_timer_cancel();
    FurnaceStatus status = furnace_get_status();
    
    BYTES_EQUAL(B00000000, virtualPORTB);
    LONGS_EQUAL(0, status.timer_remaining);
}

TEST(ControllerTests, ReceiveMessageStartTimer) {
    virtualPORTB = 0;
    
    const FurnaceStartTimerCommand fst = {'S', 120};
    
    // should invoke furnace_timer_start
    furnace_handle_incoming_msg(&fst, sizeof(FurnaceStartTimerCommand));
    
    FurnaceStatus status = furnace_get_status();
    
    BYTES_EQUAL(B00010000, virtualPORTB);
    LONGS_EQUAL(120, status.timer_remaining);
}

TEST(ControllerTests, ReceiveMessageCancelTimer) {
    virtualPORTB = 0xff;
    
    // should invoke furnace_timer_start
    furnace_handle_incoming_msg("C", 1);
    
    FurnaceStatus status = furnace_get_status();
    
    BYTES_EQUAL(B11101111, virtualPORTB);
    LONGS_EQUAL(0, status.timer_remaining);
}

TEST(ControllerTests, UpdateTimer) {
    furnace_timer_start(120);
    furnace_update_timer();
    
    FurnaceStatus status = furnace_get_status();
    
    CHECK_TRUE(status.furnace_powered);
    CHECK_TRUE(status.zone_state == ZONE_STATE_UNKNOWN);
    LONGS_EQUAL(119, status.timer_remaining);
}

TEST(ControllerTests, TimerExpired) {
    furnace_timer_start(1);
    furnace_update_timer();
    
    FurnaceStatus status = furnace_get_status();
    
    CHECK_TRUE(status.zone_state == ZONE_STATE_INACTIVE);
    LONGS_EQUAL(0, status.timer_remaining);
}

TEST(ControllerTests, SendStatus) {
    virtualPORTB = B00000000; // zone monitor inactive, furnace power on
    
    FurnaceStatus status = {ZONE_STATE_UNKNOWN, false, 0};
    
    furnace_timer_start(120);

    furnace_send_status();
    
    LONGS_EQUAL(sizeof(FurnaceStatus), spy_data_len_received);
    memcpy(&status, spy_data_received, spy_data_len_received);
    
    CHECK_TRUE(status.furnace_powered);
    CHECK_TRUE(status.zone_state == ZONE_STATE_UNKNOWN); // can't tell when timer on
    LONGS_EQUAL(120, status.timer_remaining);
}
