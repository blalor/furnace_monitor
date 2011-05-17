#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

extern "C" {
    #include <avr/io.h>
    #include <avr/interrupt.h>
    
    #include "relay.h"
}

TEST_GROUP(RelayTests) {
    void setup() {
        virtualDDRB = 0;
        virtualPORTB = 0xff;
        
        relay_init();
    }
    
    void teardown() {
        
    }
};

/*
 * confirm proper setup after initialization
 */
TEST(RelayTests, Initialization) {
    BYTES_EQUAL(B00010000, virtualDDRB);  // pin set to output
    BYTES_EQUAL(B11101111, virtualPORTB); // Relay off
}

TEST(RelayTests, RelayOn) {
    virtualPORTB = 0;
    
    relay_on();
    
    BYTES_EQUAL(B00010000, virtualPORTB); // Relay on
}

TEST(RelayTests, RelayOff) {
    virtualPORTB = 0xff;
    
    relay_off();
    
    BYTES_EQUAL(B11101111, virtualPORTB); // Relay off
}

