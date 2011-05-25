/*
    relay: PB4
    current sense: PB3
    voltage sense/zone monitor: PB2
    serial tx: PB1
    serial rx: PB0
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "8bit_tiny_timer0.h"
#include "8bit_tiny_timer1.h"
#include "usi_serial.h"
#include "serial_handler.h"
#include "scheduler.h"
#include "current_sensor.h"
#include "relay.h"
#include "zone_monitor.h"
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

const Timer1Registers timer1Regs = {
    &GTCCR,
    &TCCR1,
    &OCR1A,
    &OCR1B,
    &OCR1C,
    &TIMSK,
    &TCNT1
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

// buffer passed to serial_handler_init to store incoming data. 
// @todo tune for max expected message
#define RX_DATA_BUF_SIZE 32
uint8_t rx_data_buf[RX_DATA_BUF_SIZE];

#define TIMER1_OCRA 250

// task for sending samples; run every 15 seconds
Task sample_tx_task = {
    0,                   // counter
    7500, // secToTaskTarget(64, TIMER1_OCRA, 15), // 15 seconds
    true,                // enabled
    furnace_send_status
};

// updates the furnace timer; responsible for decrementing counter and 
// expiring when complete.
Task update_furnace_timer_task = {
    0,                  // counter
    500, // secToTaskTarget(64, TIMER1_OCRA, 1), // 1 second
    true,               // enabled
    furnace_update_timer
};

const Task *tasks[] = {
    &sample_tx_task,
    &update_furnace_timer_task,
};

int main(int argc, char **argv) {
    current_sensor_init();
    relay_init();
    zone_monitor_init();
    
    // gak. ugly inter-module dependencies. tho the order doesn't matter
    // (nothing's called until sei(), I'm trying for something that looks 
    // right.
    //     controller needs serial_handler_send
    //     usi_serial needs serial_handler_consumer, timer0
    //     serial_handler needs furnace_controller_init and usi_tx_byte

    timer0_init(&timer0Regs, TIMER0_PRESCALE_8);  // 1 µS/tick

    furnace_controller_init(serial_handler_send);
    usi_serial_init(&usiSerReg, serial_handler_consume, BAUD_9600, false);
    serial_handler_init(
        furnace_handle_incoming_msg,
        rx_data_buf, RX_DATA_BUF_SIZE,
        usi_tx_byte
    );
    
    scheduler_init(
        (Task **)&tasks,
        sizeof(tasks)/sizeof(tasks[0])
    );
    
    timer1_init(&timer1Regs, TIMER1_PRESCALE_64); // 8 µS/tick
    
    // fire scheduler_tick every 2ms (exactly!)
    timer1_attach_interrupt_ocra(TIMER1_OCRA, scheduler_tick);
    timer1_enable_ctc(TIMER1_OCRA);
    timer1_set_counter(0);
    
    timer1_start();
    
    sei();
    
    furnace_timer_start(120);

    for (;;) {
        scheduler_invoke_tasks();
        // @todo watchdog
        // @todo sleep?
    }
    
    return 0; // never reached
}
