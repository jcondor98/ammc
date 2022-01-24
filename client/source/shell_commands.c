// AVR Multi Motor Control
/*! \file client/source/shell_commands.c
 * Shell module - Commands
 *
 * \author Paolo Lucchesi
 * \todo Define various declarations and macros in a head file
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
#define psend(type, sel, data, data_size)\
  communication_craft_and_send(AVR_FD, type, sel, data, data_size)
#define precv(p) communication_recv(AVR_FD, p)


// Limit mask for DC motor IDs
#define DC_MOTOR_ID_LIMIT 127

// Type to properly handle RPM values
typedef int16_t dc_rpm_t;


// Type definition for the internal shell storage
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


// TODO: Improve
static inline uint8_t _prepare_echo_string(char *dest, int argc, char *argv[]) {
  size_t str_len = 0;
  for (int i=1; i < argc; ++i) {
    size_t tok_len = strlen(argv[i]);
    if ((str_len ? str_len+1 : 0) + tok_len >= BODY_MAX_LEN - 1)
      return 0xFF; // Maximum length for a byte, equals "too long"
    if (str_len) dest[str_len++] = ' ';
    strcpy(dest + str_len, argv[i]);
    str_len += tok_len;
  }
  dest[str_len] = '\0';
  return str_len;
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

  uint8_t str_len = _prepare_echo_string(str, argc, argv);
  sh_error_on(str_len > BODY_MAX_LEN, 2,
    "Error: arguments must be at most %d characters long " "in total\n",
    BODY_MAX_LEN - 1);

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

  const char *arg = argv[1];
  unsigned char arg_len = strlen(arg);
  sh_error_on(arg > BODY_MAX_LEN, 2, "Argument is too long");
  sh_error_on(psend(COM_TYPE_TWI_ECHO, 0, arg, arg_size) != 0, 3,
      "Could not send char to Master");

  // Get the character back from Master
  packet_t pack_rx;
  sh_error_on(precv(&pack_rx), 3, "Could not receive char back from Master");

  return 0;
}


// CMD: get-speed
// Usage: get-speed <motor-id>
// Get the speed of a DC motor in RPM
int get_speed(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc != 2) return 1;

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  unsigned char motor_id;
  int ret = sscanf(argv[1], "%hhu", &motor_id);
  sh_error_on(ret != 1 || ret >= DC_MOTOR_ID_LIMIT, 1,
      "Bad argument: %s", argv[1]);
  sh_error_on(motor_id == 0, 1, "Motor ID cannot be 0");

  // Send a GET_SPEED packet
  sh_error_on(psend(COM_TYPE_GET_SPEED, motor_id, NULL, 0) != 0, 3,
      "Could not send GET_SPEED packet to Master");

  // Wait for the DAT response
  packet_t response[1];
  sh_error_on(precv(response), 3, "Could not receive Master response");

  //! \todo Sanity check on response

  // Print the received speeds
  dc_rpm_t speed = (dc_rpm_t)(*response->body);
  printf("%hhu: %hd\n", motor_id, speed);

  return 0;
}


// CMD: set-speed
// Usage: set-speed <motor-id>=<value>
// Set the speed of a DC motor
int set_speed(int argc, char *argv[], void *storage) {
  _storage_cast(st, storage);
  if (argc != 2) return 1;

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  // Parse speed entered by user
  unsigned char motor_id;
  dc_rpm_t speed;
  int ret = sscanf(argv[1], "%hhu=%hd", &motor_id, &speed);
  sh_error_on(ret != 2 || ret >= DC_MOTOR_ID_LIMIT, 1,
      "Bad argument: %s", argv[1]);
  sh_error_on(motor_id == 0, 1, "Motor ID cannot be 0");

  // Send a SET_SPEED packet with the parsed DC motor speeds
  sh_error_on(psend(COM_TYPE_SET_SPEED, motor_id, (void*) &speed,
        sizeof(speed)) != 0, 3, "Could not send SET_SPEED packet to Master");

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


// CMD: set-slave-addr
// Usage: set-slave-addr <actual-addr> <new-addr>
// Set a new TWI address for a slave controller
int set_slave_addr(int argc, char *argv[], void *storage) {
  if (argc != 3) return 1;
  _storage_cast(st, storage);

  sh_error_on(AVR_FD < 0, 2, "Device is not connected");

  unsigned char addr_actual, addr_new;
  int ret = sscanf(argv[1], "%hhi", &addr_actual);
  ret += sscanf(argv[2], "%hhi", &addr_new);

  sh_error_on(ret != 2, 1, "Wrong address format. Use (hexa)decimal notation");
  sh_error_on(addr_actual == 0 || addr_new == 0 ||
      addr_actual >= DC_MOTOR_ID_LIMIT || addr_new >= DC_MOTOR_ID_LIMIT,
      1, "Addresses must be between 1 and %u\n", DC_MOTOR_ID_LIMIT-1);

  if (addr_actual != addr_new)
    sh_error_on(psend(COM_TYPE_SET_SLAVE_ADDR, addr_actual, &addr_new, 1) != 0,
        3, "Could not send packet to Master");

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
      "Get the speed of a DC motor in RPM\n",
    .exec = get_speed
  },

  (shell_command_t) { // CMD: set-speed
    .name = "set-speed",
    .help = "Usage: set-speed [<motor-id>=<value>]\n"
      "Set the speed of a DC motor in RPM.\n",
    .exec = set_speed
  },

  (shell_command_t) { // CMD: apply
    .name = "apply",
    .help = "Usage: apply\n"
      "Tell all the slaves to apply the previously set speed\n",
    .exec = apply
  },

  (shell_command_t) { // CMD: set-slave-addr
    .name = "set-slave-addr",
    .help = "Usage: set-slave-addr <actual-addr> <new-addr>\n"
      "Set a new TWI address for a slave controller\n",
    .exec = set_slave_addr
  },
};

// This is the exposed shell commands set
shell_command_t *shell_commands = _shell_commands;
size_t shell_commands_count = sizeof(_shell_commands) / sizeof(shell_command_t);
