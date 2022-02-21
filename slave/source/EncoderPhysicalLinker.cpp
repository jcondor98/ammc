#include <avr/interrupt.h>
#include "EncoderPhysicalLinker.h"
#include "dcmotor_phy_params.h"

static TwoPhaseEncoder *encoder;

static bool readPhaseA(void) {
  return (ENCODER_A_PIN & ENCODER_A_PIN_MASK);
}

static bool readPhaseB(void) {
  return (ENCODER_B_PIN & ENCODER_B_PIN_MASK);
}

ISR(DCMOTOR_PHY_ENCODER_ISR) {
  if (encoder) encoder->updatePosition(readPhaseA(), readPhaseB());
}

void EncoderPhysicalLinker::initialize(void) {
  ENCODER_A_DDR  &= ~ENCODER_A_DDR_MASK;
  ENCODER_B_DDR  &= ~ENCODER_B_DDR_MASK;
  ENCODER_A_PORT |= ENCODER_A_PORT_MASK;
  ENCODER_B_PORT |= ENCODER_B_PORT_MASK;
  ENCODER_PCMSK  |= ENCODER_PCMSK_MASK;
}

void EncoderPhysicalLinker::linkEncoder(TwoPhaseEncoder *enc) {
  encoder = enc;
}

void EncoderPhysicalLinker::start(void) {
  if (encoder) ENCODER_PCICR |= ENCODER_PCICR_MASK;
}

void EncoderPhysicalLinker::stop(void) {
  ENCODER_PCICR &= ~ENCODER_PCICR_MASK;
}
