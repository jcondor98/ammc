// AVR Multi Motor Control
/*! \file master/source/twi.c
 * I2C TWI Master Interface
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "twi.h"
#include "sleep_util.h"

// Mask to enable interrupts and TWI functionalities - For auxiliary functions
#define _TW_ALL_ENABLED ((1<<TWINT) | (1<<TWEN) | (1<<TWIE))

static volatile uint8_t mode = TW_READY; // TWI status
static volatile uint8_t error = TW_SUCCESS; // Current error code
static uint8_t rep_start;

// RX and TX buffers and indexes
static uint8_t *rx_buffer;
static uint8_t tx_buffer[TW_TX_MAX_LEN+1];
static volatile size_t rx_idx, tx_idx;
static size_t rx_size, tx_size;


// Send the START signal
static inline void twi_start(void)    { TWCR = _TW_ALL_ENABLED | (1<<TWSTA); }
// Send the STOP signal
static inline void twi_stop(void)     { TWCR = _TW_ALL_ENABLED | (1<<TWSTO); }
// Start or resume a data transfer
static inline void twi_transfer(void) { TWCR = _TW_ALL_ENABLED; }
// [Receiver] Resume a transfer and respond with an ACK
static inline void twi_ack(void)      { TWCR = _TW_ALL_ENABLED | (1<<TWEA); }
// [Receiver] Resume a transfer and do NOT respond with an ACK
static inline void twi_nak(void)      { TWCR = _TW_ALL_ENABLED; }


// Generate an address byte for transmitting (i.e. SLA+W)
static inline uint8_t twi_addr_write(uint8_t addr) {
  return (addr << 1) & 0xFE;
}

// Generate an address byte for receiving (i.e. SLA+R)
static inline uint8_t twi_addr_read(uint8_t addr) {
  return (addr << 1) | 0x01;
}


// Initialize the I2C TWI module
void twi_init(void) {
  TWSR = 0; // No prescaling
  TWBR = ((F_CPU / TW_FREQ) - 16) / 2; // TWI bit rate
  TWCR = (1 << TWEN); // Enable TWI functionality

  // Disable internal pull-up resistors
  DDRD  &= ~((1 << 0) | (1 << 1));
  PORTD &= ~((1 << 0) | (1 << 1));
}

// Return the TWI internal status
uint8_t twi_status(void) {
  return error;
}

// [AUX] Send "raw" data, no automatic addressing
// Do not use dedicated buffers, directly use 'tx_buffer' instead
static void _twi_send(size_t size, uint8_t bus_lock) {
  tx_size = size;
  tx_idx = 0;

  // Lock the bus after this transmission?
  rep_start = bus_lock ? 1 : 0;

  // Actually start the transmission
  if (mode == TW_REPEATED_START) {
    TWDR = tx_buffer[tx_idx++];
    twi_transfer();
  } else twi_start();
  mode = TW_INITIALIZING;
}

// Send data
uint8_t twi_send(uint8_t addr, const void *data, size_t size,
    uint8_t bus_lock) {
  if (!data || size > TW_TX_MAX_LEN) return 1;

  // Wait for the I2C channel to be ready and prepare the buffer
  sleep_while(SLEEP_MODE_IDLE, !twi_isready());
  tx_buffer[0] = twi_addr_write(addr);
  memcpy(tx_buffer + 1, data, size);

  // Actually send slave address and data
  _twi_send(size+1, bus_lock);

  return 0;
}


// Receive data from a single endpoint
uint8_t twi_recv(uint8_t addr, void *buf, size_t to_recv, uint8_t bus_lock) {
  if (!buf || to_recv > TW_RX_MAX_LEN) return 1;
  sleep_while(SLEEP_MODE_IDLE, !twi_isready());

  // Prepare buffers
  rx_buffer = buf;
  rx_size = to_recv;
  rx_idx = 0;

  // Transmit the slave address (no need to initialize TX size and index)
  tx_buffer[0] = twi_addr_read(addr);
  _twi_send(1, bus_lock);

  sleep_while(SLEEP_MODE_IDLE, !twi_isready());
  return rx_idx;
}


// Is the TWI module ready for transmitting?
uint8_t twi_isready(void) {
  return (mode == TW_READY || mode == TW_REPEATED_START) ? 1 : 0;
}

// Was the last operation successful?
uint8_t twi_successful(void) { return (error == TW_SUCCESS) ? 1 : 0; }


// TWI Interrupt Service Routine
ISR(TWI_vect) {
  uint8_t status = TW_STATUS;
  switch (status) {
    // Master Transmitter
    case TW_MT_SLA_ACK: // Address + WRITE transmitted, ACK received
      mode = TW_TRANSMITTING; // Switch mode to master transmitter
    case TW_START:  // Start condition has been transmitted
    case TW_MT_DATA_ACK: // Data byte has been transmitted, ACK received
      if (tx_idx < tx_size) { // More data to send
        TWDR = tx_buffer[tx_idx++];
        error = TW_NO_INFO;
        twi_transfer();
      }
      else if (rep_start) { // Transmission complete, send repeated start
        error = TW_SUCCESS;
        twi_start();
      }
      else { // Transmission complete, release the bus
        mode = TW_READY;
        error = TW_SUCCESS;
        twi_stop();
      }
      break;

    // Master Transmitter
    case TW_MR_DATA_ACK: // Data received, ACK transmitted
      rx_buffer[rx_idx++] = TWDR;
    case TW_MR_SLA_ACK: // Address + READ transmitted, ACK received
      mode = TW_RECEIVING;
      error = TW_NO_INFO;
      // More than one byte to read, fetch and ack the incoming byte, OR:
      // No more data can be read - Fetch the incoming byte and send NAK
      (rx_idx < rx_size - 1) ? twi_ack() : twi_nak();
      break;

    case TW_MR_DATA_NACK: // Byte received, NAK transmitted - End transmission
      rx_buffer[rx_idx++] = TWDR;
      error = TW_SUCCESS;
      if (rep_start) // Transmission complete, send repeated start
        twi_start();
      else { // All transmissions are complete, exit
        mode = TW_READY;
        twi_stop();
      }
      break;

    // Master errors
    case TW_MR_SLA_NACK: // SLA+R transmitted, NACK received
    case TW_MT_SLA_NACK: // SLA+W transmitted, NACK received
    case TW_MT_DATA_NACK: // Data byte has been transmitted, NACK received
    case TW_MT_ARB_LOST:  // Arbitration has been lost
      error = status;
      if (rep_start) // Return error and send stop and set mode to ready
        twi_start();
      else { // All transmissions are complete, exit
        mode = TW_READY;
        twi_stop();
      }
      break;

    // Miscellaneous status codes
    case TW_REP_START: // Repeated start has been transmitted
      // Set the mode but DO NOT clear TWINT as data is not ready
      mode = TW_REPEATED_START;
      break;
    case TW_NO_INFO: // Should not get into ISR under this state
      break;
    case TW_BUS_ERROR: // Illegal START/STOP, raise an error
      error = TW_BUS_ERROR;
      mode = TW_READY;
      twi_stop();
      break;
  }
}
