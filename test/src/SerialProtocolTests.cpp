#include "CppUTest/TestHarness.h"
#include "8bit_binary.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    
    void send_message(const char *msg_data, const uint8_t msg_len) {
        for (uint8_t i = 0; i < msg_len; i++) {
            serial_handler_consume(msg_data[i]);
        }
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
    send_message("\xff\x55\x04test\x3c", 8);
    
    STRCMP_EQUAL("test", (const char *)spy_msg);
}

TEST(SerialProtocolTests, ParseMessageInvalidChecksum) {
    free(spy_msg);
    spy_msg = NULL;
    
    send_message("\xff\x55\x04testc", 8);
    
    POINTERS_EQUAL(NULL, spy_msg);
}

TEST(SerialProtocolTests, ParseMessageEmbeddedStartMarker) {
    send_message("\xff\x55\x06te\xff\x55st\xe6", 10);

    STRCMP_EQUAL("te\xff\x55st", (const char *)spy_msg);
}

// more tests needed to test recovery from bad message
TEST(SerialProtocolTests, RecoveryFromLongMessage) {
    // send message with invalid data length and checksum
    send_message("\xff\x55\x06waytoolong\xe6", 14);
    
    // valid message
    // ow, my brain!
    // "\xff\x55\x07f" is being parsed as 0xff 0x55 0x7f! gcc bug?
    send_message(
        (const char[]){0xff, 0x55, 0x07, 'f', 'o', 'o', ' ', 'b', 'a', 'r', 0x60},
        11
    );
    
    STRCMP_EQUAL("foo bar", (const char *)spy_msg);
}

TEST(SerialProtocolTests, RecoveryFromShortMessage) {
    // send message with invalid data length and checksum
    // 0x14 == 20, data is only 10 bytes long (11 w/ checksum); so header and
    // data length of next message will be swallowed and lost
    // NOTE: I don't like this; feels too clunky. Maybe add a scheduler task,
    // if the protocol can't be improved upon?
    send_message("\xff\x55\x14waytoolong\xe6", 14);
    
    // subsequent message will be lost
    send_message("\xff\x55\x08testtest\x78", 12);
    
    // valid message
    // ow, my brain!
    // "\xff\x55\x07f" is being parsed as 0xff 0x55 0x7f! gcc bug?
    send_message(
        (const char[]){0xff, 0x55, 0x07, 'f', 'o', 'o', ' ', 'b', 'a', 'r', 0x60},
        11
    );
    
    STRCMP_EQUAL("foo bar", (const char *)spy_msg);
}
