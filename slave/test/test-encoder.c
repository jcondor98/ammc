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

volatile float measured_speed;
volatile uint8_t pid_triggered;


ISR(DCMOTOR_PHY_ENCODER_ISR) {
  uint8_t enc_a = dcmotor_phy_read_encoder_phase_a();
  uint8_t enc_b = dcmotor_phy_read_encoder_phase_b();
  if (enc_a) enc_b ? ++position : --position;
}

static inline float compute_speed_from_position(int32_t _position) {
  return position / DC_SAMPLING_INTERVAL
      * 60000 / DC_ENC_SIGNALS_PER_ROUND;
}

ISR(DCMOTOR_PHY_PID_ISR) {
  measured_speed = compute_speed_from_position(position);
  position = 0;
  pid_triggered = 1;
}

static void dump_position(position_t position) {
  static char msg[0xFF];
  uint8_t msg_len = sprintf(msg, "Position: %ld\n", position);
  serial_tx(msg, msg_len);
}

static void dump_speed(float speed) {
  static char msg[0xFF];
  uint8_t rpm = (uint8_t) measured_speed;
  uint8_t msg_len = sprintf(msg, "Measured speed: %hhu\n", rpm);
  serial_tx(msg, msg_len);
}

int main(void) {
  dcmotor_phy_encoder_init();
  dcmotor_phy_pid_init(DC_SAMPLING_INTERVAL);
  serial_init();
  sei();

  dcmotor_phy_pid_start();
  uint16_t ocr_value = (OCR1AH << 8) | OCR1AL;
  serial_tx(&ocr_value, sizeof(ocr_value));
  while (1) {
    /*
    dump_position(position);
    dump_speed(measured_speed);
    _delay_ms(DELAY_INTERVAL_MS);
    */
    if (pid_triggered) {
      pid_triggered = 0;
      dump_speed(measured_speed);
    }
  }
}

/*
static inline void led_init(void)   { DDRB  |= 1 << 7; }
static inline void led_toggle(void) { PORTB ^= 1 << 7; }
*/
