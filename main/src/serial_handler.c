#include "serial_handler.h"

#include <stdint.h>
#include <stdio.h>

typedef enum __state {
    STATE_WAITING_FOR_HDR,
    STATE_WAITING_FOR_LEN,
    STATE_WAITING_FOR_CHKSUM,
} State;

// circular-buffer-like thing
#define HDR_BUF_SIZE 2
static uint8_t hdr_buf[HDR_BUF_SIZE];
static uint8_t hdr_ind;

static State state;
static uint8_t msg_len;
static uint8_t checksum;

static void (*msg_handler)(const uint8_t *, const uint8_t);

// buffer for storing incoming data
static uint8_t *data_buf;
static uint8_t data_buf_len, data_buf_ind;

void serial_handler_init(
    void (*received_msg_handler)(const uint8_t *, const uint8_t),
    uint8_t *buf,
    uint8_t buf_len
) {
    state = STATE_WAITING_FOR_HDR;
    hdr_ind = 0;
    
    msg_handler = received_msg_handler;
    
    data_buf = buf;
    data_buf_len = buf_len;
    data_buf_ind = 0;
}

void serial_handler_consume(const uint8_t byte) {
    // hdr_ind and hdr_ind_prev used to test last two bytes received against 
    // start marker (0xff 0x55)
    uint8_t hdr_ind_prev = hdr_ind;
    hdr_ind = (hdr_ind + 1) % HDR_BUF_SIZE;
    
    // add incoming byte to header buffer
    hdr_buf[hdr_ind] = byte;
    
    // test for start marker in incoming data
    if ((hdr_buf[hdr_ind_prev] == 0xff) && (hdr_buf[hdr_ind] == 0x55)) {
        state = STATE_WAITING_FOR_LEN;
        
        data_buf_ind = 0;
        msg_len = 0;
    }
    else if (state == STATE_WAITING_FOR_LEN) {
        // record length of the message, minus the checksum
        msg_len = byte;
        
        if (msg_len <= data_buf_len) {
            // can proceed to next state
            state = STATE_WAITING_FOR_CHKSUM;
            
            checksum = msg_len;
        }
        else {
            // buffer's not big enough
            state = STATE_WAITING_FOR_HDR;
        }
    }
    else if (state == STATE_WAITING_FOR_CHKSUM) {
        // waiting for checksum
        if (data_buf_ind < msg_len) {
            data_buf[data_buf_ind++] = byte;
            checksum += byte;
        }
        else {
            // received all data; this is the checksum
            if (((0x100 - (checksum & 0xFF))) == byte) {
                // successful!
                msg_handler(data_buf, msg_len);
            }
            
            state = STATE_WAITING_FOR_HDR;
        }
    }
}
