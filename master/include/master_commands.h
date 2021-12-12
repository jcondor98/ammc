// AVR Multi Motor Control
/*! \file master/include/master_commands.h
 * DC Motor Master Command Module
 *
 * \author Paolo Lucchesi
 */
#ifndef __MASTER_COMMANDS_MODULE_H
#define __MASTER_COMMANDS_MODULE_H
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
typedef enum _MASTER_COMMAND_ID_E {
  CMD_GET_SPEED,   // Get the dc motor speed
  CMD_SET_SPEED,   // Set a new speed for the dc motor
  CMD_APPLY_SPEED, // Apply the previously set speed
  CMD_ECHO,        // [Debug] Echo a single character via I2C
  CMD_SET_ADDR     // Change the slave current address
} master_command_id_t;


/*!
 * Send a command to a slave
 *
 * @param slave_id The id of the slave
 * @param id The command id
 * @param arg The command argument
 * @param arg_size The command argument size
 */
void master_send_command(
    uint8_t slave_id,
    uint8_t id,
    void *arg,
    uint8_t arg_size
);

/*!
 * Send a command in broadcast to all slaves
 *
 * @param id The command id
 * @param arg The command argument
 * @param arg_size The command argument size
 */
void master_send_bcast_command(uint8_t id, void *arg, uint8_t arg_size);

/*
 * Receive a command response from a slave
 *
 * @param slave_id The id of the slave
 * @param buf The buffer to store the response
 * @param size The expected (or maximum) size of the response
 * @returns The actual size of the response
 */
uint8_t master_recv_response(uint8_t slave_id, void *buf, uint8_t size);

#endif	// __MASTER_COMMANDS_MODULE_H
