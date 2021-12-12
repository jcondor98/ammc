// AVR Multi Motor Control
/*! \file slave/source/command.c
 * Master->Slave ammc commands
 *
 * \author Paolo Lucchesi
 */
#include "command.h"
#include "dcmotor.h"
#include "twi.h"


// Returns !0 if the command is valid and sane, 0 otherwise
uint8_t command_isvalid(const master_command_t *cmd, uint8_t cmd_size) {
  if (!cmd) return 0;
  switch (cmd->id) {
    case CMD_GET_SPEED:
    case CMD_APPLY_SPEED:
      return cmd_size == 1;
    case CMD_SET_SPEED:
      return cmd_size == 1 + sizeof(dc_rpm_t);
    case CMD_ECHO:
      return 1;
    case CMD_SET_ADDR:
      uint8_t new_addr = *((uint8_t*)(cmd->argument));
      return (cmd_size == 2 && new_addr != 0 && new_addr < 127);
    default: return 0; // Unknown command
  }
}

uint8_t command_recv(master_command_t *buf) {
  return twi_recv(buf, TW_RX_MAX_LEN);
}

uint8_t command_respond(const void *response, uint8_t size) {
  return twi_send(response, size);
}
