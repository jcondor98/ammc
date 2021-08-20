// AVR Multi Motor Control -- Paolo Lucchesi
// Serial interface - Head file
// NOTE: Uses POSIX standard read() and write() for communication
#ifndef __SERIAL_MODULE_H
#define __SERIAL_MODULE_H
#include <unistd.h>
#include <termios.h>
#include "packet.h"
#include "ringbuffer.h"

#define BAUD_RATE B115200

#if defined(DEBUG) && !defined(RX_BUF_SIZE)
#define RX_BUF_SIZE 512
#elif !defined(RX_BUF_SIZE)
#define RX_BUF_SIZE 64
#endif


// Open a serial device
// Returns a file descriptor to the open device, or -1 on error
int serial_open(const char *dev);

// Close a serial device
// Returns 0 on success, 1 if the descriptor given is not a tty device,
// -1 if the 'close' function fails
int serial_close(int dev_fd);

// Get at most 'n' bytes from the serial port
// Return the number of bytes read
ssize_t serial_rx(int dev_fd, void *dest, size_t size);

// Get a single character
#define serial_rx_getchar(ctx,dst) serial_rx(ctx,dst,1)

// Write data to the serial port with POSIX write
// Return the number of bytes effectively written or -1 on failure
ssize_t serial_tx(int dev_fd, const void *buf, size_t size);

// Put a single character
#define serial_tx_putchar(ctx,src) serial_tx(ctx,src,1)

// Flush the RX, TX or both queue(s)
// Forward the return value given by tcflush()
#define serial_rx_flush(fd) tcflush(fd,TCIFLUSH)
#define serial_tx_flush(fd) tcflush(fd,TCOFLUSH)
#define serial_flush(fd) tcflush(fd,TCIOFLUSH)

#endif  // __SERIAL_MODULE_H
