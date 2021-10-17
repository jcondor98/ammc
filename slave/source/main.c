// AVR Multi Motor Control
/*! \file slave/source/main.c
 * Slave Main routine
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include "sleep_util.h"
#include "dcmotor.h"
#include "twi.h"

// TWI Slave own address
// TODO: Set this dynamically (e.g. via EEPROM)
#define TW_OWN_ADDRESS 0x31

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
  power_setup();
  dcmotor_init();
  twi_init(TW_OWN_ADDRESS);
  sei();

  //dcmotor_pid_start();

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
        PORTB |= 1 << 5;
        twi_send(rx_buf + 1, 1);
        break;

      default: break; // Unknown command, simply ignore
    }
  }
}
