// AVR Multi Motor Control
/*! \file include/common/master_commands.h
 * DC Motor Master Command common definitions
 *
 * \author Paolo Lucchesi
 */
#ifndef __MASTER_COMMANDS_MODULE_COMMON_H
#define __MASTER_COMMANDS_MODULE_COMMON_H
#include <stdint.h>

#define COMMAND_MAX_SIZE 16
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
typedef enum _MASTER_COMMAND_ID_E {
  CMD_GET_SPEED,   // Get the dc motor speed
  CMD_SET_SPEED,   // Set a new speed for the dc motor
  CMD_APPLY_SPEED, // Apply the previously set speed
  CMD_PING,        // Ping a slave controller
  CMD_SET_ADDR     // Change the slave current address
} master_command_id_t;

#endif	// __MASTER_COMMANDS_MODULE_COMMON_H
