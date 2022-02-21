#include "pid_physical_linker.h"
#include "dcmotor_phy_params.h"

#define OCR_ONE_MSEC 15.625

static DCMotor *motor;

ISR(DCMOTOR_PHY_PID_ISR) {
  if (motor) motor->adjustActualSpeed();
}

void PidPhysicalLinker::initialize(uint16_t intervalInMillis) {
  uint16_t ocrValue = intervalInMillis * OCR_ONE_MSEC;
  PID_TCCRA = PID_TCCRA_VALUE;
  PID_TCCRB = PID_TCCRB_VALUE;
  PID_OCRH = ocrValue >> 8;
  PID_OCRL = ocrValue & 0x00FF;
}

void PidPhysicalLinker::start(void) {
  if (!motor) return;
  PID_TCNT = 0;
  PID_TIMSK |= PID_TIMSK_MASK;
}

void PidPhysicalLinker::stop(void) {
  PID_TIMSK &= ~PID_TIMSK_MASK;
}

void PidPhysicalLinker::linkMotor(DCMotor *m) {
  motor = m;
}
