// AVR Multi Motor Control
/*! \file master/source/master_commands.c
 * DC Motor Master Command Module
 *
 * \author Paolo Lucchesi
 */
#include <string.h>
#include "master_commands.h"
#include "twi.h"

static inline void prepare_command(master_command_t *command, uint8_t id,
    const void *arg, uint8_t arg_size) {
  memcpy(&command->id, &id, sizeof(id));
  if (arg) memcpy(command->argument, arg, arg_size);
}

void master_send_command(uint8_t slave_id, uint8_t id,
    const void *arg, uint8_t arg_size) {
  master_command_t command[1];
  prepare_command(command, id, arg, arg_size);
  twi_send_sm(slave_id, command, sizeof(id) + arg_size);
}

void master_send_bcast_command(uint8_t id, const void *arg, uint8_t arg_size) {
  master_command_t command[1];
  prepare_command(command, id, arg, arg_size);
  twi_bcast_sm(command, sizeof(id) + arg_size);
}

uint8_t master_recv_response(uint8_t slave_id, void *buf, uint8_t size) {
  return twi_recv_sm(slave_id, buf, size);
}
