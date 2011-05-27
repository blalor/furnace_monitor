/*
    Very simple program that outputs samples of the data structures.  I'm never
    quite certain of the byte ordering (tho it appears to be little endian) and
    I want to make certain that the enums are decoded correctly on the 
    receiving end.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>

#include "8bit_tiny_timer0.h"
#include "usi_serial.h"
#include "serial_handler.h"

#include "controller.h"

const Timer0Registers timer0Regs = {
    &GTCCR,
    &TCCR0A,
    &TCCR0B,
    &OCR0A,
    &TIMSK,
    &TIFR,
    &TCNT0
};

const USISerialRegisters usiSerRegs = {
    &PORTB,
    &PINB,
    &DDRB,
    &USIBR,
    &USICR,
    &USIDR,
    &USISR,
    &GIFR,
    &GIMSK,
    &PCMSK,
};

static int uart_putchar(char c, FILE *stream) {
    return usi_tx_byte((uint8_t) c);
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main(int argc, char **argv) {
    timer0_init(&timer0Regs, TIMER0_PRESCALE_8);  // 1 µS/tick
    
    usi_serial_init(&usiSerRegs, NULL, BAUD_9600, false);
    stdout = &mystdout;

    serial_handler_init(
        NULL,
        NULL, 0,
        usi_tx_byte
    );

    sei();
    
    FurnaceStatus status;
    
    for (;;) {
        puts(">> start (zone_state,furnace_powered,timer_remaining)");
        
        // -----
        puts("ZONE_STATE_UNKNOWN,false,0");
        status.zone_state = ZONE_STATE_UNKNOWN;
        status.furnace_powered = false;
        status.timer_remaining = 0;
        
        serial_handler_send(&status, sizeof(FurnaceStatus));

        // -----
        puts("ZONE_STATE_ACTIVE,false,0");
        status.zone_state = ZONE_STATE_ACTIVE;
        
        serial_handler_send(&status, sizeof(FurnaceStatus));

        // -----
        puts("ZONE_STATE_INACTIVE,true,0xABCD");
        status.zone_state = ZONE_STATE_INACTIVE;
        status.furnace_powered = true;
        status.timer_remaining = 0xABCD; // easily decoded on the receiving end
        
        serial_handler_send(&status, sizeof(FurnaceStatus));

        puts("<< end");
        _delay_ms(500);
    }

    return 0; // never reached
}
