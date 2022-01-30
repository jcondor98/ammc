// AVR Multi Motor Control
/*! \file master/include/twi.h
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

//! Status code for TWI successful operation
#define TW_SUCCESS 0xFF

//! TWI mode (i.e. status)
typedef enum _TWI_MODE_E {
  TW_READY,           //!< I2C module ready
  TW_REPEATED_START,  //!< I2C module ready, slave already addressed by repeated start
  TW_INITIALIZING,    //!< Setting up for I2C communicaton session
  TW_TRANSMITTING,    //!< Transmitting data
  TW_RECEIVING        //!< Receiving data
} twi_mode_t;


//! Initialize the I2C TWI module
void twi_init(void);

/*!
 * Is the TWI module ready for tranceiving?
 *
 * @return 1 if ready, 0 if busy
 */
uint8_t twi_isready(void);

/*!
 * Was the last operation successful?
 *
 * @return 1 if successful, 0 otherwise
 */
uint8_t twi_successful(void);

//! @return The TWI internal status
uint8_t twi_status(void);

/*!
 * Send data
 *
 * If bus_lock != 0, a repeated start will be sent and the bus will be
 * locked by the current master device.
 * General call (i.e. broadcast) address 0x00 can be used.
 * \attention Locks if the bus is busy
 *
 * @param addr Slave address
 * @param data Pointer to the data to be transmitted
 * @param size Size of 'data'
 * @param bus_lock Lock the bus sending repeated starts after finishing? (1 if yes)
 * @return 0 on success, 1 on error
 */
uint8_t twi_send(uint8_t addr, const void *data, size_t size, uint8_t bus_lock);

/*!
 * Receive data from a single endpoint
 *
 * If bus_lock != 0, a repeated start will be sent and the bus will be
 * locked by the current master device.
 * \attention Locks if the bus is busy
 *
 * @param addr Slave address
 * @param buf  Pointer to the buffer to store the received data
 * @param to_recv Maximum number of data bytes to receive
 * @param bus_lock Lock the bus sending repeated starts after finishing? (1 if yes)
 * @return The number of effectively received bytes
 */
uint8_t twi_recv(uint8_t addr, void *buf, size_t to_recv, uint8_t bus_lock);

//! Send data, single master configurations (never lock the bus)
#define twi_send_sm(addr,data,size) twi_send(addr,data,size,0)

//! Receive data, single master configurations (never lock the bus)
#define twi_recv_sm(addr,data,size) twi_recv(addr,data,size,0)

//! Send data in broadcast. \attention Slaves can not acknowledge
#define twi_bcast(data,size,bus_lock) twi_send(0x00,data,size,bus_lock)

//! Send data in broadcast, single master. \attention Slaves can not acknowledge
#define twi_bcast_sm(data,size) twi_bcast(data,size,0)

#endif  // __I2C_MASTER_MODULE_H
