#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

extern "C" {
    #include <avr/io.h>
    
    #include "current_sensor.h"
}

TEST_GROUP(CurrentSensorTests) {
    void setup() {
        virtualDDRB = 0xff;
        
        current_sensor_init();
    }
    
    void teardown() {
        
    }
};

/*
 * confirm proper setup after initialization
 */
TEST(CurrentSensorTests, Initialization) {
    BYTES_EQUAL(B11110111, virtualDDRB);  // pin set to input
}

TEST(CurrentSensorTests, CurrentFlowing) {
    virtualPORTB = B11110111;
    
    CHECK_TRUE(is_current_flowing());
}

TEST(CurrentSensorTests, CurrentIsNotFlowing) {
    virtualPORTB = B11111111;
    
    CHECK_FALSE(is_current_flowing());
}
