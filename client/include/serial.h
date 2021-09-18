// AVR Multi Motor Control
/*! \file client/include/serial.h
 * Serial interface
 *
 * \note Uses POSIX standard read() and write() for communication
 * \author Paolo Lucchesi
 */
#ifndef __SERIAL_MODULE_H
#define __SERIAL_MODULE_H
#include <unistd.h>
#include <termios.h>
#include "packet.h"
#include "ringbuffer.h"

//! Serial port baud rate
#define BAUD_RATE B115200

#if defined(DEBUG) && !defined(RX_BUF_SIZE)
#define RX_BUF_SIZE 512
#elif !defined(RX_BUF_SIZE)
#define RX_BUF_SIZE 64
#endif


/*!
 * Open a serial device
 *
 * @param dev Path to the serial port to open
 * @returns a file descriptor to the open device, or -1 on error
 */
int serial_open(const char *dev);

/*!
 * Close a serial device
 *
 * @param dev_fd Pointer to the serial port to close
 * @returns 0 on success, 1 if the descriptor given is not a tty device,
 * -1 if the 'close' function fails
 */
int serial_close(int dev_fd);

/*!
 * Get at most 'n' bytes from the serial port
 *
 * @param dev_fd Pointer to the serial port
 * @param dest   Buffer to store the received data
 * @param size   Number of bytes to receive
 * @returns The number of bytes read
 */
ssize_t serial_rx(int dev_fd, void *dest, size_t size);

//! Get a single character
#define serial_rx_getchar(ctx,dst) serial_rx(ctx,dst,1)

/*!
 * Write data to the serial port with POSIX write
 *
 * @param dev_fd Pointer to the serial port
 * @param buf    Buffer containing the data to transmit
 * @param size   Number of bytes to transmit
 * @returns The number of bytes effectively written or -1 on failure
 */
ssize_t serial_tx(int dev_fd, const void *buf, size_t size);

//! Put a single character
#define serial_tx_putchar(ctx,src) serial_tx(ctx,src,1)

/*!
 * Flush the RX queue
 *
 * @param fd File descriptor for the serial port
 * @returns The return value given by tcflush()
 */
#define serial_rx_flush(fd) tcflush(fd,TCIFLUSH)

/*!
 * Flush the TX queue
 *
 * @param fd File descriptor for the serial port
 * @returns The return value given by tcflush()
 */
#define serial_tx_flush(fd) tcflush(fd,TCOFLUSH)

/*!
 * Flush the RX and TX queue
 *
 * @param fd File descriptor for the serial port
 * @returns The return value given by tcflush()
 */
#define serial_flush(fd) tcflush(fd,TCIOFLUSH)

#endif  // __SERIAL_MODULE_H
