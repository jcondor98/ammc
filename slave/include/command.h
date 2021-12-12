// AVR Multi Motor Control
/*! \file slave/include/command.h
 * Master->Slave ammc commands
 *
 * \author Paolo Lucchesi
 */
#ifndef __MASTER_TO_SLAVE_COMMANDS_H
#define __MASTER_TO_SLAVE_COMMANDS_H
#include "common/master_commands.h"
#include "twi.h"

/*!
 * Receive a command
 *
 * @param buf The buffer for receiving the command
 * @returns The size of the received command
 */
uint8_t command_recv(master_command_t *buf);

/*!
 * Respond to a previously sent command
 *
 * @param response The response to the command
 * @param size The response size
 * @returns E_SUCCESS on success, another error value otherwise
 */
uint8_t command_respond(const void *response, uint8_t size);

//! @returns !0 if the command is valid and sane, 0 otherwise
uint8_t command_isvalid(const master_command_t *cmd, uint8_t cmd_size);

#endif  // __MASTER_TO_SLAVE_COMMANDS_H
