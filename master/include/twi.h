// AVR Multi Motor Control -- Paolo Lucchesi
// I2C TWI Master Interface - Head file
#ifndef __I2C_MASTER_MODULE_H
#define __I2C_MASTER_MODULE_H
#include <stdint.h>
#include <stddef.h>
#include <util/twi.h>

// TWI Bitrate Frequency
#define TW_FREQ 100000

// Max len for buffers
#define TW_RX_MAX_LEN 32
#define TW_TX_MAX_LEN 32

// Code for TWI successful operation
#define TW_SUCCESS 0xFF

// TWI status
typedef enum _TWI_MODE_E {
  TW_READY,
  TW_REPEATED_START,
  TW_INITIALIZING,
  TW_TRANSMITTING,
  TW_RECEIVING
} twi_mode_t;


// Initialize the I2C TWI module
void twi_init(void);

// Is the TWI module ready for transmitting?
uint8_t twi_isready(void);

// Was the last operation successful?
uint8_t twi_successful(void);

// Send and receive data from a single endpoint
// If bus_lock != 0, a repeated start will be sent and the bus will be
// locked by the current master device
// Lock if the bus is busy
// Return 0 on success, 1 on error
uint8_t twi_send(uint8_t addr, const void *data, size_t size, uint8_t bus_lock);
uint8_t twi_recv(uint8_t addr, void *buf, size_t to_recv, uint8_t bus_lock);

// Send and receive data, single master configurations (never lock the bus)
#define twi_send_sm(addr,data,size) twi_send(addr,data,size,0)
#define twi_recv_sm(addr,data,size) twi_recv(addr,data,size,0)

// Send data in broadcast -- Beware: slaves can not acknowledge
// Return 0 on success, 1 on error
#define twi_bcast(data,size,bus_lock) twi_send(0x00,data,size,bus_lock)
#define twi_bcast_sm(data,size) twi_bcast(data,size,0)

#endif  // __I2C_MASTER_MODULE_H
