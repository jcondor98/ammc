// AVR Multi Motor Control
/*! \file slave/source/main.c
 * Slave Main routine
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/power.h>
#include <avr/eeprom.h>

#include "sleep_util.h"
#include "dcmotor.h"
#include "twi.h"

//! TWI default address (likely to be changed)
#ifndef TW_DEFAULT_ADDRESS
#define TW_DEFAULT_ADDRESS 0x01
#endif

// TWI Slave own address, dynamically loaded from EEPROM
uint8_t EEMEM twi_own_address = TW_DEFAULT_ADDRESS;


// Salviamo gli alberi insieme
static inline void power_setup(void) {
  // Disable unneeded modules
  power_spi_disable();
  power_timer1_disable();
  power_usart0_disable();

  // Power off the built-in LED on pin D13
  DDRB |= 1 << 5;
  PORTB &= ~(1 << 5);
}


int main(void) {
  // Setup and initialize all modules
  power_setup();
  dcmotor_init();
  sei();

  // Load slave own address and initialize the TWI module
  uint8_t own_addr = eeprom_read_byte(&twi_own_address);
  twi_init(own_addr);

  // Start Proportional-Integral-Derivative controller for the DC motor
  dcmotor_pid_start();

  uint8_t rx_buf[TW_RX_MAX_LEN]; // TWI RX buffer

  // Main application loop
  while (1) {
    // Get command from Master via TWI
    uint8_t received = twi_recv(rx_buf, TW_RX_MAX_LEN);

    // Execute the received command
    switch (rx_buf[0]) {
      case DC_MOTOR_CMD_GET:
        dc_rpm_t rpm_actual = dcmotor_get();
        twi_send(&rpm_actual, sizeof(dc_rpm_t));
        break;

      case DC_MOTOR_CMD_SET:
        dc_rpm_t *rpm_next = (dc_rpm_t*)(rx_buf + 1); // First byte is command
        dcmotor_set(*rpm_next);
        break;

      case DC_MOTOR_CMD_APPLY:
        dcmotor_apply();
        break;

      case TWI_CMD_ECHO: // Used for debug
        PORTB |= 1 << 5; // Turn on embedded LED (show that TWI is receiving)
        twi_send(rx_buf + 1, 1);
        break;

      case TWI_CMD_SET_ADDR:
        if (received != 2 || rx_buf[1] == 0 || rx_buf[1] > 126) break;
        uint8_t new_addr = rx_buf[1];
        eeprom_write_byte(&twi_own_address, new_addr);
        // Apply changes immediately
        own_addr = new_addr;
        twi_init(new_addr);
        break;

      default: break; // Unknown command, simply ignore
    }
  }
}
