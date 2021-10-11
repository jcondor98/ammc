// AVR Multi Motor Control
/*! \file client/include/communication.h
 * Communication layer
 *
 * \author Paolo Lucchesi
 */
#ifndef __COMMUNICATION_MODULE_H
#define __COMMUNICATION_MODULE_H
#include "packet.h"
#include "serial.h"

//! Precise error state codes for receiving/sending errors
typedef enum ERR_CODE_E {
  E_SUCCESS = 0, E_ID_MISMATCH, E_CORRUPTED_CHECKSUM, E_WRONG_TYPE
} err_code_t;


/*!
 * Setup a connection, sending a handshake (HND) packet
 *
 * @param dev_fd File descriptor for the device
 * @returns 0 on success, 1 on failure
 */
int communication_connect(int dev_fd);

/*!
 * Send a packet
 *
 * @param dev_fd File descriptor for the device
 * @param p      Pointer to the packet to send
 * @returns 0 on success, 1 on failure
 * \warning Never use for HND, ACK or ERR packet types
 */
int communication_send(int dev_fd, const packet_t *p);

/*!
 * Receive a packet
 *
 * @param dev_fd File descriptor for the device
 * @param p      Pointer to a preallocated struct to store the received packet
 * @returns 0 on success, 1 on failure
 * \warning Never use for HND, ACK or ERR packet types
 */
int communication_recv(int dev_fd, packet_t *p);

/*!
 * Craft a packet in-place and send it
 *
 * @param dev_fd    File descriptor for the device
 * @param type      Packet type
 * @param selector  Selector
 * @param data      Data to store in the packet body
 * @param data_size Size of 'data' in bytes
 * @returns 0 on success, 1 on failure
 * \warning Never use for HND, ACK or ERR packet types
 */
int communication_craft_and_send(int dev_fd, unsigned char type,
    unsigned char selector, const unsigned char *data,
    unsigned char data_size);

#endif   // __COMMUNICATION_MODULE_H
