// AVR Multi Motor Control
/*! \file master/include/serial.h
 * Serial communication layer
 *
 * \author Paolo Lucchesi
 */
#ifndef __SERIAL_MODULE_H
#define __SERIAL_MODULE_H
#include <stdint.h>

//! USART Baud Rate
#define BAUD_RATE 115200

//! 16-bit value for the UBRR register
#define UBRR_VALUE (F_CPU / 8 / BAUD_RATE - 1)

//! Transmission buffer size - Default is 64
#ifndef TX_BUFFER_SIZE
#define TX_BUFFER_SIZE 32
#endif

//! Reception buffer size - Default is 64
#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE 32
#endif


//! Initialize the UART
void serial_init(void);

/*!
 * Read 'size' bytes
 *
 * @param buf Pointer to the buffer to store the data
 * @param Size of 'buf'
 * @return The number of bytes read
 */
uint8_t serial_rx(void *buf, uint8_t size);

/*!
 * Read at most 'size' bytes, non blocking
 *
 * @param buf Pointer to the buffer to store the data
 * @param Size of 'buf'
 * @return The number of bytes read
 */
uint8_t serial_rx_nonblock(void *buf, uint8_t size);

//! Return a single received character
#define serial_rx_getchar(c) serial_rx(c,1)

/*!
 * Send data stored in a buffer
 *
 * The data will be copied into another buffer, so it can be reused immediately.
 * The function blocks until the previous transmission, if any, is completed,
 * and returns immediately, not waiting for all the data to be already sent
 *
 * @param buf Pointer to the data to send
 * @param size Number of bytes to send
 * @return 0 on success, 1 on failure
 */
uint8_t serial_tx(const void *buf, uint8_t size);

//! @return The number of bytes received
uint8_t serial_rx_available(void);

//! @return The number of bytes sent
uint8_t serial_tx_sent(void);

//! @return 1 if RX is ongoing, 0 otherwise
uint8_t serial_rx_ongoing(void);

//! @return 1 if TX is ongoing, 0 otherwise
uint8_t serial_tx_ongoing(void);

//! Reset the RX state of the serial interface
void serial_rx_reset(void);

//! Reset the TX state of the serial interface
void serial_tx_reset(void);

#endif    // __SERIAL_MODULE_H
