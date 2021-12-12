// AVR Multi Motor Control
/*! \file master/include/master_commands.h
 * DC Motor Master Command Module
 *
 * \author Paolo Lucchesi
 */
#ifndef __MASTER_COMMANDS_MODULE_H
#define __MASTER_COMMANDS_MODULE_H
#include "common/master_commands.h"
#include "twi.h"

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
