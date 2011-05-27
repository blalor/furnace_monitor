#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

extern "C" {
    #include <avr/io.h>
    
    #include "furnace_power_sensor.h"
}

TEST_GROUP(FurnacePowerSensorTests) {
    void setup() {
        virtualDDRB = 0xff;
        
        furnace_power_sensor_init();
    }
    
    void teardown() {
        
    }
};

/*
 * confirm proper setup after initialization
 */
TEST(FurnacePowerSensorTests, Initialization) {
    BYTES_EQUAL(B11110111, virtualDDRB);  // pin set to input
}

TEST(FurnacePowerSensorTests, CurrentFlowing) {
    virtualPINB = B11110111;
    
    CHECK_TRUE(is_furnace_power_on());
}

TEST(FurnacePowerSensorTests, CurrentIsNotFlowing) {
    virtualPINB = B11111111;
    
    CHECK_FALSE(is_furnace_power_on());
}
