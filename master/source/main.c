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
static uint8_t action_performed;


static inline void builtin_led_off(void) {
  DDRB |= 1 << 7;
  PORTB &= ~(1 << 7);
}

static inline void disable_analog_pins_io(void) {
  DIDR0 = 0xFF;
  DIDR1 = 0xFF;
  DIDR2 = 0xFF;
  DIDR0 = 0xFF;
}

static inline void jtag_disable(void) {
  uint8_t mcucr_bak = MCUCR | (1 << JTD);
  MCUCR = 1 << JTD;
  MCUCR = 1 << JTD;
  MCUCR = mcucr_bak;
}

// Salviamo gli alberi insieme
static inline void power_setup(void) {
  disable_analog_pins_io();
  power_timer0_disable();
  power_timer2_disable();
  power_timer5_disable();
  power_usart1_disable();
  power_usart2_disable();
  power_spi_disable();
  builtin_led_off();
  jtag_disable();
}


static inline void setup() {
  serial_init();
  twi_init();
  sei();
}

int main(void) {
  power_setup();

  while (1) { // Main application loop
    perform_setup = 0;
    setup();

    while (!perform_setup) {
      action_performed = 0;
      action_performed |= communication_handler();

      // Enter sleep mode if no action was or is to be performed
      sleep_on(SLEEP_MODE_IDLE, !action_performed);
    }
  }
}
