// AVR Multi Motor Control
/*! \file client/source/shell_commands.c
 * Shell module - Commands
 *
 * \author Paolo Lucchesi
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "shell.h"
#include "serial.h"
#include "communication.h"
#include "debug.h"


// Declare a shell_storage_t variable casted from an opaque pointer
#define _storage_cast(st,opaque) shell_storage_t *st=(shell_storage_t*)(opaque)

// Handier function to send and receive packets
#define AVR_FD (st->avr_fd) // Must be the context in EVERY function below
#define psend(type,sel,data,data_size)\
  communication_craft_and_send(AVR_FD,type,sel,data,data_size)
#define precv(p) communication_recv(AVR_FD,p)


// Number of DC motors
// TODO: Define this in a head file
#define DC_MOTOR_NUM 4

// Type to properly handle RPM values
// TODO: Define this in a head file
typedef uint8_t dc_rpm_t;


// Type definition for the internal shell storage
// TODO: Other application stuff to add?
typedef struct _shell_storage_s {
  int avr_fd;
} shell_storage_t;


// Allocate and initialize a shell storage
// Returns an opaque pointer to the allocated storage, or NULL on failure
void *shell_storage_new(void) {
  shell_storage_t *st = malloc(sizeof(shell_storage_t));
  if (!st) return NULL;

  AVR_FD = -1;  // i.e. not connected

  return (void*) st;
}

// Cleanup this shell environment (i.e. free memory, close descriptors...)
// Also frees the shell storage data structure itself
void shell_cleanup(shell_t *s) {
  if (!s) return;
  _storage_cast(st, s->storage);

  // Close the connection with the device, if any
  if (AVR_FD > 0 && serial_close(AVR_FD) != 0)
    err_log("Could not close device file descriptor");

  // Free the storage
  free(st);
}



// CMD: connect
// Usage: connect <device_path>
// Connect to an ammc device, given its dev file (e.g. /dev/ttyACM0)
// If it is already connected, reconnect
int connect(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc < 2) return 1;

  if (AVR_FD > 0)
    fputs("Open serial context found; reconnecting\n", stderr);
  else { // Open a descriptor for the device
    AVR_FD = serial_open(argv[1]);
    sh_error_on(AVR_FD < 0, 2, "Unable to connect to device");
  }

  // Estabilish the connection
  sh_error_on(communication_connect(AVR_FD) != 0, 3,
      "Could not send a handshake packet");

  return 0;
}


// CMD: disconnect
// Usage: disconnect
// Close an existing connection - Has no effect on the device
int disconnect(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc != 1) return 1;
  sh_error_on(AVR_FD < 0, 2, "device is not connected");

  int ret = serial_close(AVR_FD);
  AVR_FD = -1; // i.e. set to disconnected
  sh_error_on(ret != 0, 3, "Could not close device file descriptor");

  return 0;
}


// CMD: dev-echo
// Usage: dev-echo <arg> [arg2 arg3 ...]
// Send a (null terminated) string to the device, which should send it back
// Primarily used for debugging purpose
int dev_echo(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc < 2) return 1;
  char str[BODY_MAX_LEN - 1];

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  // Reassemble the separated argv[] token into one string
  size_t str_len = 0;
  for (int i=1; i < argc; ++i) {
    size_t tok_len = strlen(argv[i]);
    sh_error_on(
        (str_len ? str_len+1 : 0) + tok_len >= BODY_MAX_LEN - 1, 2,
        "Error: arguments must be at most %d characters long " "in total\n",
        BODY_MAX_LEN - 1);
    if (str_len) str[str_len++] = ' ';
    strcpy(str + str_len, argv[i]);
    str_len += tok_len;
  }
  str[str_len] = '\0';

  // Send the string to the device
  sh_error_on(psend(COM_TYPE_ECHO, 0, (const void*) str, str_len) != 0, 3,
      "Could not send the string to echo to device");

  // Get the string back from the device
  packet_t pack_rx;
  sh_error_on(precv(&pack_rx), 3,
      "Could not receive the echo string back from the device");

  // Check the packet data size and prepare the received string to be printed
  const unsigned char pack_rx_size_expected = sizeof(header_t) +
    str_len + sizeof(crc_t);
  const unsigned char pack_rx_size = packet_get_size(&pack_rx);
  sh_error_on(pack_rx_size != pack_rx_size_expected, 2,
      "Error: The received packet is %hhu bytes long, expected %hhu\n",
      pack_rx_size, pack_rx_size_expected);
  pack_rx.body[str_len] = '\0'; // Substitute first CRC byte with string terminator

  // Print the received string and return
  puts((char*) pack_rx.body);
  return 0;
}


// CMD: dev-echo-twi
// Usage: dev-echo-twi <char>
// Send a character to the Master, which will forward it to Slave and expect
// a response. Used to test the TWI interface
int dev_echo_twi(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc != 2) return 1;

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  // Send the character to Master
  sh_error_on(psend(COM_TYPE_ECHO_TWI, 0, (const void*) argv[1], 1) != 0, 3,
      "Could not send char to Master");

  // Get the character back from Master
  packet_t pack_rx;
  sh_error_on(precv(&pack_rx), 3, "Could not receive char back from Master");

  return 0;
}


// CMD: get-speed
// Usage: get-speed [motor-id]
// Get the speed of a DC motor in RPM, or of all the motors if no arguments
// Multiple space-separated motors can be specified
int get_speed(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc > 5) return 1;

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  // Decode motors identifiers to selector masks
  unsigned char selector = 0;
  if (argc == 1) selector = ~0;
  else for (unsigned char i=1; i < argc; ++i) {
    unsigned char motor_id;
    int ret = sscanf(argv[i], "%hhu", &motor_id);
    sh_error_on(ret != 1 || ret >= DC_MOTOR_NUM, 1,
        "Bad argument: %s", argv[i]);
    selector |= 1 << motor_id;
  }

  // Send a GET_SPEED packet
  sh_error_on(psend(COM_TYPE_GET_SPEED, selector, NULL, 0) != 0, 3,
      "Could not send GET_SPEED packet to Master");

  // Wait for the DAT response
  packet_t response[1];
  sh_error_on(precv(response), 3, "Could not receive Master response");

  // TODO: Sanity check on response

  // Print the received speeds
  dc_rpm_t *speeds = (dc_rpm_t*) response->body;
  for (unsigned char i=0; i < DC_MOTOR_NUM; ++i)
    if (selector & (1 << i)) printf("%hhu: %hhu\n", i, speeds[i]);

  return 0;
}


// CMD: set-speed
// Usage: set-speed [<motor-id>=<value>]
// Set the speed of a DC motor. Multiple space-separated motors and values
// can be specified
int set_speed(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc < 2) return 1;

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  // Store user-specified speeds for the DC motors
  dc_rpm_t speeds[DC_MOTOR_NUM];

  // Decode motors identifiers to selector masks
  unsigned char selector = 0;
  for (unsigned char i=1; i < argc; ++i) {
    unsigned char motor_id;
    dc_rpm_t rpm;
    int ret = sscanf(argv[i], "%hhu=%hhu", &motor_id, &rpm);
    sh_error_on(ret != 1 || ret >= DC_MOTOR_NUM, 1,
        "Bad argument: %s", argv[i]);
    selector |= 1 << motor_id;
    speeds[motor_id] = rpm;
  }

  // Send a SET_SPEED packet with the parsed DC motor speeds
  sh_error_on(psend(COM_TYPE_SET_SPEED, selector, (void*) speeds,
        sizeof(speeds)) != 0, 3, "Could not send GET_SPEED packet to Master");

  return 0;
}


// CMD: apply
// Usage: apply
// Tell all the slaves to apply the previously set speed
int apply(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc != 1) return 1;

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  // Send an APPLY packet
  sh_error_on(psend(COM_TYPE_APPLY, 0, NULL, 0) != 0, 3,
      "Could not send APPLY packet to Master");

  return 0;
}



// Set of all the shell commands
static shell_command_t _shell_commands[] = {
  (shell_command_t) { // CMD: connect
    .name = "connect",
    .help = "Usage: connect <device_path>\n"
      "Connect to an ammc device, given its dev file (e.g. /dev/ttyACM0)\n"
      "If it is already connected, reconnect",
    .exec = connect
  },

  (shell_command_t) { // CMD: disconnect
    .name = "disconnect",
    .help = "Usage: disconnect\n"
      "Close an existing connection - Has no effect on the device",
    .exec = disconnect
  },

  (shell_command_t) { // CMD: dev-echo
    .name = "dev-echo",
    .help = "Usage: dev-echo <arg> [arg2 arg3 ...]\n"
            "Send a string to the device, which should send it back",
    .exec = dev_echo
  },

  (shell_command_t) { // CMD: dev-echo-twi
    .name = "dev-echo-twi",
    .help = "Usage: dev-echo-twi <char>\n"
      "Send a character to the Master, which will forward it to Slave and expect\n"
      "a response. Used to test the TWI interface\n",
    .exec = dev_echo_twi
  },

  (shell_command_t) { // CMD: get-speed
    .name = "get-speed",
    .help = "Usage: get-speed [motor-id]\n"
      "Get the speed of a DC motor in RPM, or of all the motors if no arguments\n"
      "Multiple space-separated motors can be specified (to a maximum of 4)\n",
    .exec = get_speed
  },

  (shell_command_t) { // CMD: set-speed
    .name = "set-speed",
    .help = "Usage: set-speed [<motor-id>=<value>]\n"
      "Set the speed of a DC motor in RPM. Multiple space-separated motors and values\n"
      "can be specified\n",
    .exec = set_speed
  },

  (shell_command_t) { // CMD: apply
    .name = "apply",
    .help = "Usage: apply\n"
      "Tell all the slaves to apply the previously set speed\n",
    .exec = apply
  },
};

// This is the exposed shell commands set
shell_command_t *shell_commands = _shell_commands;
size_t shell_commands_count = sizeof(_shell_commands) / sizeof(shell_command_t);
