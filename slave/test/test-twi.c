// AVR Multi Motor Control
// TWI test firmware for slave controller
//#include "serial.h"
#include <avr/interrupt.h>
#include "twi.h"

#define TWI_SLAVE_ADDR 0x01

#define INPUT_MAX_SIZE 1
char input[INPUT_MAX_SIZE];

int main(void) {
  twi_init(TWI_SLAVE_ADDR);
  sei();

  // Initialize built-in LED
  DDRB |= 1 << 7;
  PORTB &= ~(1 << 7);

  while (1) {
    uint8_t received = twi_recv(input, 1);
    if (received == 1 && input[0] == 'A') // Arbitrary condition to turn on LED
      PORTB |= 1 << 7;
    twi_send(input, 1);
  }
}
