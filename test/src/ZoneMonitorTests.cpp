#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

extern "C" {
    #include <avr/io.h>
    
    #include "zone_monitor.h"
}

TEST_GROUP(ZoneMonitorTests) {
    void setup() {
        virtualDDRB = 0xff;
        
        zone_monitor_init();
    }
    
    void teardown() {
        
    }
};

/*
 * confirm proper setup after initialization
 */
TEST(ZoneMonitorTests, Initialization) {
    BYTES_EQUAL(B11111011, virtualDDRB);  // pin set to input
}

TEST(ZoneMonitorTests, ZoneInactive) {
    virtualPINB = B11111011; // voltage present, LED lit, output low
    
    CHECK_FALSE(is_zone_active());
}

TEST(ZoneMonitorTests, ZoneActive) {
    virtualPINB = B11111111; // no voltage present, LED unlit, output high
    
    CHECK_TRUE(is_zone_active());
}
