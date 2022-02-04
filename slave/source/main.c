// AVR Multi Motor Control
/*! \file slave/source/main.c
 * Slave Main routine
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/power.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "sleep_util.h"
#include "command.h"
#include "dcmotor.h"
#include "twi.h"


#define TW_DEFAULT_ADDRESS 0x01
uint8_t EEMEM twi_own_address_eeprom = TW_DEFAULT_ADDRESS;
uint8_t twi_own_address;


static inline void change_twi_address_to(uint8_t new_addr) {
  eeprom_write_byte(&twi_own_address_eeprom, new_addr);
  twi_own_address = new_addr;
  twi_init(new_addr);
}

static inline void execute_command(const master_command_t *cmd, uint8_t cmd_size) {
  if (!command_isvalid(cmd, cmd_size)) return;
  switch (cmd->id) {
    case CMD_GET_SPEED:
      dc_rpm_t rpm_actual = dcmotor_get();
      command_respond(&rpm_actual, sizeof(dc_rpm_t));
      break;
    case CMD_SET_SPEED:
      dc_rpm_t new_speed = *((dc_rpm_t*)(cmd->argument));
      dcmotor_set(new_speed);
      break;
    case CMD_APPLY_SPEED:
      dcmotor_apply();
      break;
    case CMD_ECHO: // Used for debug
      command_respond(cmd->argument, cmd_size);
      break;
    case CMD_SET_ADDR:
      uint8_t new_addr = *((uint8_t*)(cmd->argument));
      change_twi_address_to(new_addr);
      break;
    default: break; // Unknown command, simply ignore
  }
}

static inline void power_saving_setup(void) {
  power_spi_disable();
  power_usart0_disable();
}

static inline void setup(void) {
  power_saving_setup();
  dcmotor_init();
  sei();

  twi_own_address = eeprom_read_byte(&twi_own_address_eeprom);
  twi_init(twi_own_address);

  dcmotor_phy_pid_start();
}


int main(void) {
  setup();

  master_command_t command[1];
  while (1) {
    uint8_t command_size = command_recv(command);
    execute_command(command, command_size);
  }
}
