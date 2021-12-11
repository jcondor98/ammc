// AVR Multi Motor Control
/*! \file slave/source/main.c
 * Slave Main routine
 *
 * \author Paolo Lucchesi
 * \todo Abstract master commands
 */
#include <avr/io.h>
#include <avr/power.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "sleep_util.h"
#include "dcmotor.h"
#include "twi.h"


#define TW_DEFAULT_ADDRESS 0x01
uint8_t EEMEM twi_own_address_eeprom = TW_DEFAULT_ADDRESS;
uint8_t twi_own_address;


// Salviamo gli alberi insieme
static inline void power_setup(void) {
  // Disable unneeded modules
  power_spi_disable();
  //power_timer1_disable();
  //power_usart0_enable();

  // Power off the built-in LED on pin D13
  //DDRB |= 1 << 5;
  //PORTB &= ~(1 << 5);
}


static inline void change_twi_address_to(uint8_t new_addr) {
  eeprom_write_byte(&twi_own_address_eeprom, new_addr);
  twi_own_address = new_addr;
  twi_init(new_addr);
}

static inline void execute_master_command(const uint8_t *cmd, uint8_t cmd_size) {
  uint8_t command_id = cmd[0];
  switch (command_id) {
    case DC_MOTOR_CMD_GET:
      dc_rpm_t rpm_actual = dcmotor_get();
      twi_send(&rpm_actual, sizeof(dc_rpm_t));
      break;
    case DC_MOTOR_CMD_SET:
      dc_rpm_t *rpm_next = (dc_rpm_t*)(cmd + 1); // First byte is command
      dcmotor_set(*rpm_next);
      break;
    case DC_MOTOR_CMD_APPLY:
      dcmotor_apply();
      break;
    case TWI_CMD_ECHO: // Used for debug
      twi_send(cmd + 1, 1);
      break;
    case TWI_CMD_SET_ADDR:
      if (! (cmd_size != 2 || cmd[1] == 0 || cmd[1] > 126))
        change_twi_address_to(cmd[1]);
      break;
    default: break; // Unknown command, simply ignore
  }
}


int main(void) {
  // Setup and initialize all modules
  power_setup();
  //dcmotor_init();
  sei();

  twi_own_address = eeprom_read_byte(&twi_own_address_eeprom);
  twi_init(twi_own_address);

  // Start Proportional-Integral-Derivative controller for the DC motor
  //dcmotor_pid_start();

  uint8_t command[TW_RX_MAX_LEN]; // TWI RX buffer

  // Main application loop
  while (1) {
    uint8_t command_size = twi_recv(command, TW_RX_MAX_LEN);
    execute_master_command(command, command_size);
  }
}
