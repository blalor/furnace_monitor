#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
    #include "serial_handler.h"
}

static uint8_t *spy_msg;

#define DATA_BUF_SIZE 50
static uint8_t data_buf[DATA_BUF_SIZE];

void spy_receive_msg(const uint8_t *msg, const uint8_t len) {
    free(spy_msg);
    spy_msg = (uint8_t *)malloc(len + 5);
    strncpy((char *)spy_msg, (char *)msg, len);
}

TEST_GROUP(SerialProtocolTests) {
    void setup() {
        spy_msg = (uint8_t *)malloc(1);
        memset(spy_msg, 0, 1);

        serial_handler_init(spy_receive_msg, data_buf, DATA_BUF_SIZE);
    }
    
    void teardown() {
        free(spy_msg);
        spy_msg = NULL;
    }
};

/*
The following Python snippet will generate the checksum for `data'

    chksum = len(data)
    for b in data:
        chksum += ord(b)

    chksum = (0x100 - (chksum & 0xFF))
*/

TEST(SerialProtocolTests, ParseMessage) {
    const char *msg = "\xff\x55\x04test\x3c";
    
    for (uint8_t i = 0; i < sizeof(msg)/sizeof(msg[0]); i++) {
        serial_handler_consume(msg[i]);
    }
    
    STRCMP_EQUAL("test", (const char *)spy_msg);
}

TEST(SerialProtocolTests, ParseMessageInvalidChecksum) {
    free(spy_msg);
    spy_msg = NULL;
    
    const char *msg = "\xff\x55\x04testc";
    
    for (uint8_t i = 0; i < sizeof(msg)/sizeof(msg[0]); i++) {
        serial_handler_consume(msg[i]);
    }
    
    POINTERS_EQUAL(NULL, spy_msg);
}

/*
    Hm, this test passes.  That kind of sucks.  How to handle binary data, then?
*/
TEST(SerialProtocolTests, ParseMessageEmbeddedStartMarker) {
    free(spy_msg);
    spy_msg = NULL;

    const char *msg = "\xff\x55\x06te\xff\x55st\xe6";
    
    for (uint8_t i = 0; i < sizeof(msg)/sizeof(msg[0]); i++) {
        serial_handler_consume(msg[i]);
    }
    
    POINTERS_EQUAL(NULL, spy_msg);
}
