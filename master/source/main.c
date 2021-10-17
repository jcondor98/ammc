// AVR Multi Motor Control
/*! \file master/source/main.c
 * Main routine
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include "sleep_util.h"
#include "serial.h"
#include "communication.h"
#include "twi.h"


// Perform setup routine?
static uint8_t perform_setup = 1;

// Counter for actions performed by the handlers
// Shall be changed to 'volatile' if interrupts will be involved in future
static uint8_t act_perf;


// Salviamo gli alberi insieme
static inline void power_setup(void) {
  // Disable unneeded modules
  power_spi_disable();
  power_timer0_disable();
  power_timer2_disable();
  power_timer5_disable();
  power_usart1_disable();
  power_usart2_disable();

  // Disable digital I/O for analog pins
  DIDR0 = 0xFF;
  DIDR1 = 0xFF;
  DIDR2 = 0xFF;

  // Power off the built-in LED on pin D13
  DDRB |= 1 << 7;
  PORTB &= ~(1 << 7);

  // Disable On-Chip Debug module (i.e. JTAG, must be done in 4 cycles to work)
  uint8_t mcucr_bak = MCUCR | (1 << JTD);
  MCUCR = 1 << JTD;
  MCUCR = 1 << JTD;
  MCUCR = mcucr_bak;
}


int main(void) {
  power_setup();

  while (1) { // Main application loop
    serial_init();
    twi_init();
    perform_setup = 0;
    sei();

    // Main application loop
    while (!perform_setup) {
      act_perf = 0;
      act_perf |= communication_handler();

      // Enter (interruptable) sleep mode if no action request was performed
      // AVR automatically wakes up from idle mode on incoming data from serial
      sleep_on(SLEEP_MODE_IDLE, !act_perf);
    }
  }
}
