// AVR Multi Motor Control
/*! \file slave/include/command.h
 * Master->Slave ammc commands
 *
 * \author Paolo Lucchesi
 */
#ifndef __MASTER_TO_SLAVE_COMMANDS_H
#define __MASTER_TO_SLAVE_COMMANDS_H
#include "twi.h"

#define COMMAND_MAX_SIZE TW_RX_MAX_LEN
#define COMMAND_ARG_MAX_SIZE (COMMAND_MAX_SIZE-1)

//! Data type for a master command
typedef struct _master_command_s {
  uint8_t id;
  uint8_t argument[COMMAND_ARG_MAX_SIZE];
} master_command_t;


/*!
 * \enum _DC_MOTOR_CMD_E
 * \brief Master -> Slave commands
 */
typedef enum _DC_MOTOR_CMD_E {
  CMD_GET_SPEED,   // Get the dc motor speed
  CMD_SET_SPEED,   // Set a new speed for the dc motor
  CMD_APPLY_SPEED, // Apply the previously set speed
  CMD_ECHO,        // [Debug] Echo a single character via I2C
  CMD_SET_ADDR     // Change the slave current address
} dc_motor_cmd_t;

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
