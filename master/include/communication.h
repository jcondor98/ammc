// AVR Multi Motor Control
/*! \file master/include/communication.h
 * Communication module (server)
 *
 * \author Paolo Lucchesi
 */
#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H
#include <stdint.h>
#include "packet.h"

/*!
 *  Create (in-place) and send a packet
 *
 *  @param type       Packet type
 *  @param selector   Selector field value
 *  @param body_size  The size of the body
 *  @param body       A pointer to the body data memory area
 */
void communication_send(uint8_t type, uint8_t selector,
    uint8_t body_size, const void *body);

/*!
 *  Receive a packet
 *  \todo Flush serial on header error
 *
 *  @param type       Packet type
 *  @param selector   Selector field value
 *  @param body_size  The size of the body
 *  @param body       A pointer to the body data memory area
 *  @returns E_SUCCESS on success, or a value related to the failure otherwise
 */
com_error_t communication_recv(packet_t *rx);

/*!
 *  Communication handler routine
 *
 *  As long as it's called again, it is safe to return across different states
 *  as the execution will be resumed
 *
 *  @returns 0 if no action was performed or must be performed, 1 otherwise
 */
uint8_t communication_handler(void);

#endif	// __COMMUNICATION_H
