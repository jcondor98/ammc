// AVR Multi Motor Control
// TWI test firmware for master controller
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "serial.h"
#include "twi.h"

#define TWI_SLAVE_ADDR 0x01
const char *msg_listening = "Listening...\n";
const char *msg_received = "Received message: ";
const char *msg_echoed = "Echoed message: ";
const char *msg_newline = "\n";

#define INPUT_MAX_SIZE 1
char input[INPUT_MAX_SIZE];
char output[INPUT_MAX_SIZE];

static inline void newline(void) {
  serial_tx(msg_newline, 1);
}

int main(void) {
  serial_init();
  twi_init();
  sei();

  while (1) {
    serial_tx(msg_listening, strlen(msg_listening));
    uint8_t received = serial_rx(input, INPUT_MAX_SIZE);

    serial_tx(msg_received, strlen(msg_received));
    serial_tx(input, received);
    newline();

    uint8_t status;
    uint8_t send_ret = twi_send_sm(TWI_SLAVE_ADDR, input, received);
    status = twi_status();
    serial_tx(&send_ret, 1);
    serial_tx(&status, 1);

    _delay_ms(100);

    uint8_t recv_ret = twi_recv_sm(TWI_SLAVE_ADDR, output, received);
    status = twi_status();
    serial_tx(&recv_ret, 1);
    serial_tx(&status, 1);

    serial_tx(msg_echoed, strlen(msg_echoed));
    serial_tx(output, received);
    newline();
  }
}
