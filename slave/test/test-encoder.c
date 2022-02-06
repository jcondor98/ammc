// AVR Multi Motor Control
// DC Motor encoder test firmware
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"
#include "dcmotor_phy_params.h"
#include "serial.h"

#define DELAY_INTERVAL_MS 2500

typedef int32_t position_t;
position_t position;

static inline void led_init(void)   { DDRB  |= 1 << 7; }
static inline void led_toggle(void) { PORTB ^= 1 << 7; }

ISR(DCMOTOR_PHY_ENCODER_ISR) {
  uint8_t enc_a = dcmotor_phy_read_encoder_phase_a();
  uint8_t enc_b = dcmotor_phy_read_encoder_phase_b();

  if (enc_a == 0) return; // Alter only on rising edge
  enc_b ? ++position : --position;
  if (enc_b) led_toggle();
}

void dump_position(position_t position) {
  static char msg[0xFF];
  uint8_t msg_len = sprintf(msg, "Position: %ld\n", position);
  serial_tx(msg, msg_len);
}

int main(void) {
  dcmotor_phy_encoder_init();
  serial_init();
  led_init();
  sei();

  while (1) {
    dump_position(position);
    _delay_ms(DELAY_INTERVAL_MS);
  }
}
