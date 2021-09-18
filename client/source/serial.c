// AVR Multi Motor Control
/*! \file client/source/serial.c
 * Serial interface
 *
 * \author Paolo Lucchesi
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "serial.h"

// Check if a device file descriptor is valid or not
static inline int valid_dev(int dev_fd) {
  return dev_fd > 2 && isatty(dev_fd) ? 1 : 0;
}


// Open a serial device
// Returns 0 on success, 1 on failure
int serial_open(const char *dev) {
  if (!dev || *dev == '\0') return 1;
  int dev_fd = -1;

  // Open the device file
  if ((dev_fd = open(dev, O_RDWR | O_NOCTTY)) < 0) {
    perror(__func__);
    return -1;
  }

  struct termios dev_io;
  tcgetattr(dev_fd, &dev_io);

  // Serial communication parameters
  // Completely raw communication, 8N1, no special and escape characters,
  // no echo
  dev_io.c_iflag &= ~(IXON | IXOFF | IXANY | BRKINT | ICRNL | IMAXBEL);
  dev_io.c_iflag |= IGNBRK;
  dev_io.c_oflag = 0;
  dev_io.c_cflag &= ~(CRTSCTS | PARENB | CSTOPB | CSIZE);
  dev_io.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHOCTL
      | ECHOKE | ISIG | ICANON | IEXTEN);
  dev_io.c_cflag |= CS8 | CREAD | CLOCAL; // 8N1

  // No default serial special characters
  memset(&dev_io.c_cc, '\0', sizeof(dev_io.c_cc));

  // Baud rate
  cfsetospeed(&dev_io, BAUD_RATE);
  cfsetispeed(&dev_io, BAUD_RATE);

  dev_io.c_cc[VMIN] = 1;
  dev_io.c_cc[VTIME] = 1;

  // Attrbutes are applied immediately
  tcsetattr(dev_fd, TCSANOW, &dev_io);
  tcflush(dev_fd, TCIFLUSH);

  return dev_fd;
}


// Close a serial device
// Returns 0 on success, 1 if the descriptor given is not a tty device,
// -1 if the 'close' function fails
int serial_close(int dev_fd) {
  return (!valid_dev(dev_fd)) ? 1 : close(dev_fd);
}


// Receive data from the serial port
// Returns the number of read bytes, or -1 on failure
ssize_t serial_rx(int dev_fd, void *dest, size_t size) {
  if (!valid_dev(dev_fd) || !dest) return 0;
  unsigned bytes_read = 0;

  while (bytes_read < size) {
    ssize_t r = read(dev_fd, dest + bytes_read, size - bytes_read);
    if (r < 0) return -1;
    else bytes_read += r;
  }

  return size;
}


// Send data to the serial port
// Returns the number of written bytes, or -1 on failure
ssize_t serial_tx(int dev_fd, const void *buf, size_t size) {
  if (!valid_dev(dev_fd)) return -1;
  unsigned written = 0;

  while (written < size) {
    ssize_t r = write(dev_fd, buf + written, size - written);
    if (r < 0) return -1;
    else written += r;
  }

  tcdrain(dev_fd);
  return size;
}
