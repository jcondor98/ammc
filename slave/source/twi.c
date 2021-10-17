// AVR Multi Motor Control
/*! \file slave/source/twi.c
 * I2C TWI Master Interface
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "twi.h"

// Mask to enable interrupts and TWI functionalities - For auxiliary functions
#define _TW_ALL_ENABLED ((1<<TWINT) | (1<<TWEN) | (1<<TWIE))

static volatile uint8_t mode = TW_READY; // TWI status
static volatile uint8_t error = TW_SUCCESS; // Current error code

// RX and TX buffers and indexes
static uint8_t *rx_buffer;
static uint8_t tx_buffer[TW_TX_MAX_LEN];
static volatile size_t rx_idx, tx_idx;
static size_t rx_size, tx_size;


// Start or resume a data transfer
static inline void twi_transfer(void) { TWCR = _TW_ALL_ENABLED; }
// [Receiver] Resume a transfer and respond with an ACK
static inline void twi_ack(void)      { TWCR = _TW_ALL_ENABLED | (1<<TWEA); }
// [Receiver] Resume a transfer and do NOT respond with an ACK
static inline void twi_nak(void)      { TWCR = _TW_ALL_ENABLED; }


// Initialize the I2C TWI module
void twi_init(uint8_t slave_addr) {
  TWSR = 0; // No prescaling
  TWBR = ((F_CPU / TW_FREQ) - 16) / 2; // TWI bit rate
  TWAR = (slave_addr << 1) | 1; // Own slave address (accept broadcast)
  TWCR = 1 << TWEN; // TODO: Check

  // Disable internal pull-up resistors
  DDRC  &= ~((1 << 4) | (1 << 5));
  PORTC &= ~((1 << 4) | (1 << 5));
}


// Send data
uint8_t twi_send(const void *data, size_t size) {
  if (!data || size > TW_TX_MAX_LEN) return 1;

  // Wait for the I2C channel to be ready and prepare the buffer
  while (!twi_isready()) _delay_us(1); // TODO: Pause instead of delaying
  memcpy(tx_buffer, data, size);
  tx_size = size;
  tx_idx = 0;

  // Actually start the transmission
  mode = TW_INITIALIZING;
  twi_ack();

  return 0;
}


// Receive data
uint8_t twi_recv(void *buf, size_t to_recv) {
  if (!buf || to_recv > TW_RX_MAX_LEN) return 1;

  // Wait for the I2C channel to be ready
  while (!twi_isready()) _delay_us(1); // TODO: Pause instead of delaying
  mode = TW_INITIALIZING;

  // Prepare buffers
  rx_buffer = buf;
  rx_size = to_recv;
  rx_idx = 0;

  // Get ready for the transfer and wait for it to be completed
  twi_ack();
  while (!twi_isready()) _delay_us(1); // TODO: Pause instead of delaying
  return rx_idx;
}


// Is the TWI module ready for transmitting?
uint8_t twi_isready(void) { return (mode == TW_READY) ? 1 : 0; }

// Was the last operation successful?
uint8_t twi_successful(void) { return (error == TW_SUCCESS) ? 1 : 0; }


// Interrupt Service Routine
ISR(TWI_vect) {
  uint8_t status = TW_STATUS; // Get TWI status from status register
  switch (status) {
    // Slave transmitter
    case TW_ST_SLA_ACK: // SLA+R received, ACK returned
      mode = TW_TRANSMITTING;
    case TW_ST_DATA_ACK: // Data byte transmitted, ACK received
      error = TW_SUCCESS;
      TWDR = tx_buffer[tx_idx++];
      twi_ack();
      break;
    case TW_ST_DATA_NACK: // Data byte transmitted, NAK received
      error = (tx_idx >= tx_size) ? TW_SUCCESS : status;
      mode = TW_READY;
      //twi_ack();
      TWCR = 1 << TWEN;
      break;

    // Slave receiver
    case TW_SR_SLA_ACK:  // SLA+W received, ACK returned
    case TW_SR_GCALL_ACK: // Broadcast address received, ACK returned
      mode = TW_RECEIVING;
      twi_ack();
      break;
    case TW_SR_DATA_ACK:  // Data received, ACK returned
    case TW_SR_GCALL_DATA_ACK: // Data received (broadcast), ACK returned
      // TODO: Check out-of-bound index?
      rx_buffer[rx_idx++] = TWDR;
      error = TW_NO_INFO;
      twi_ack();
      break;
    case TW_SR_STOP: // STOP/REPSTART condition received while still addressed as Slave
      mode = TW_READY;
      //twi_ack();
      TWCR = 1 << TWEN;
      break;

    // Slave errors
    case TW_ST_LAST_DATA:  // Last byte transmitted, ACK received
    case TW_SR_DATA_NACK:  // Data received, NAK returned
    case TW_SR_GCALL_DATA_NACK: // Data received (broadcast), NAK returned
    case TW_BUS_ERROR: // Illegal START/STOP, raise an error
      error = status;
      mode = TW_READY;
      TWCR = (1 << TWEN) | (1 << TWSTO); // Release bus
      break;

    // Miscellaneous status codes
    case TW_NO_INFO: // Should not get into ISR under this state
      break;
    case TW_SR_ARB_LOST_SLA_ACK: // SLA+W received, Master lost arbitration
    case TW_SR_ARB_LOST_GCALL_ACK: // Broadcast address received, Master lost arbitration, ACK returned
    case TW_ST_ARB_LOST_SLA_ACK: // SLA+R received, Master lost arbitration
      error = status;
      mode = TW_READY;
      //twi_ack();
      TWCR = 1 << TWEN;
      break;
  }
}