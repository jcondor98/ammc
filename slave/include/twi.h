// AVR Multi Motor Control
/*! \file slave/include/twi.h
 * I2C TWI Master Interface
 *
 * \author Paolo Lucchesi
 */
#ifndef __I2C_MASTER_MODULE_H
#define __I2C_MASTER_MODULE_H
#include <stdint.h>
#include <stddef.h>
#include <util/twi.h>

//! TWI Bitrate Frequency
#define TW_FREQ 100000

//! Max length for RX buffer
#define TW_RX_MAX_LEN 32

//! Max length for TX buffer
#define TW_TX_MAX_LEN 32

//! Code for TWI Successful operation
#define TW_SUCCESS 0xFF

//! TWI status
typedef enum _TWI_MODE_E {
  TW_READY,           // I2C module ready
  TW_INITIALIZING,    // Setting up for I2C communicaton session
  TW_TRANSMITTING,    // Transmitting data
  TW_RECEIVING        // Receiving data
} twi_mode_t;


/*!
 * Initialize the I2C TWI module
 *
 * @param slave_addr The slave address, not shifted and without R/W bit
 */
void twi_init(uint8_t slave_addr);

/*!
 * Is the TWI module ready for transmitting?
 *
 * @returns 1 if the TWI module is ready, 0 if not
 */
uint8_t twi_isready(void);

/*!
 * Send data from a single endpoint
 *
 * @param data Pointer to the buffer filled with the data to send
 * @param size Number of bytes to send
 * @returns 0 on success, 1 on error
 */
uint8_t twi_send(const void *data, size_t size);

/*!
 * Receive data from a single endpoint
 *
 * @param buf     Pointer to the buffer to store the received data
 * @param to_recv Number of bytes to receive
 * @returns 0 on success, 1 on error
 */
uint8_t twi_recv(void *buf, size_t to_recv);

/*!
 * Was the last operation successful?
 *
 * @returns 1 if the last operation was successful, 0 if not
 */
uint8_t twi_successful(void);

#endif  // __I2C_MASTER_MODULE_H
