/*
    relay: PB4
    current sense: PB3
    voltage sense/zone monitor: PB2
    serial tx: PB1
    serial rx: PB0
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>

#include "8bit_tiny_timer0.h"
#include "usi_serial.h"

const Timer0Registers timer0Regs = {
    &GTCCR,
    &TCCR0A,
    &TCCR0B,
    &OCR0A,
    &TIMSK,
    &TIFR,
    &TCNT0
};

const USISerialRegisters usiSerReg = {
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

volatile bool have_byte;
volatile uint8_t received_byte;

static int uart_putchar(char c, FILE *stream) {
    return usi_tx_byte(c);
}

static FILE mystdout =
    FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void handle_received_byte(uint8_t b) {
    received_byte = b;
    have_byte = true;
    // PORTB ^= _BV(PB4);
}

int main(int argc, char **argv) {
    timer0_init(&timer0Regs, TIMER0_PRESCALE_8); //  1 µS/tick
    usi_serial_init(&usiSerReg, handle_received_byte, BAUD_9600, false);

    stdout = &mystdout;
    
    // DDRB |= _BV(PB4);
    // PORTB |= _BV(PB4);
    
    sei();
    
    for (;;) {
        // if (have_byte) {
        //     uint8_t b = received_byte;
        //     have_byte = false;
        //     
        //     (void) usi_tx_byte(b);
        // }
        _delay_ms(1000);
        printf("Hello, world!\n");

        // (void) usi_tx_byte('f');
        // (void) usi_tx_byte('o');
        // (void) usi_tx_byte('o');
        // (void) usi_tx_byte('\n');
    }
    
    return 0; // never reached
}
