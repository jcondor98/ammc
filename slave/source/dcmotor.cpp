// AVR Multi Motor Control
/*! \file slave/source/dcmotor.c
 * DC Motor Slave Module
 * \todo Use 16-bit PWM
 *
 * \author Paolo Lucchesi
 */
#include <avr/interrupt.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"
#include "dcmotor_phy_params.h"
#include "dcmotor.h"
#include "dcmotor_pid.h"

// TODO: Make these dynamic and configurable
#define DCMOTOR_PID_KP 1.5
#define DCMOTOR_PID_KI 3.7
#define DCMOTOR_PID_KD 0.0

#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))

namespace Phy = PhysicalLayer;


DCMotor::DCMotor(void)
  : phy(PhysicalLayer::defaultPort),
    pid({
      DCMOTOR_PID_KP,
      DCMOTOR_PID_KI,
      DCMOTOR_PID_KD,
      DC_SAMPLING_INTERVAL
    }),
    encoder({}),
    actualSpeed(0),
    targetSpeed(0),
    nextSpeed(0) {
}

void DCMotor::initializePhysicalLayer(void) {
  phy.initialize();
  EncoderPhysicalLinker::initialize();
  PidPhysicalLinker::initialize(DC_SAMPLING_INTERVAL);
}

void DCMotor::linkToPhysicalLayer() {
  phy.loadSpeed(0);
  EncoderPhysicalLinker::linkEncoder(encoder);
  PidPhysicalLinker::linkMotor(this);
}

static inline dc_rpm_t computeLimitedSpeed(dc_rpm_t speed) {
  return (speed >= 0)
    ? min(speed, DC_MOTOR_MAX_RPM_SPEED)
    : max(speed, DC_MOTOR_MAX_RPM_SPEED);
}

void DCMotor::setTargetSpeed(dc_rpm_t next) {
  nextSpeed = computeLimitedSpeed(next);
}

void DCMotor::applyTargetSpeed(void) {
  targetSpeed = nextSpeed;
}

dc_rpm_t DCMotor::getActualSpeed(void) {
  return actualSpeed;
}

void DCMotor::adjustActualSpeed(void) {
  float speed = encoder.sampleSpeed();
  float corrected_speed = pid.correct(speed, targetSpeed);

  phypwm.loadSpeed(corrected_speed);
  actualSpeed = (dc_rpm_t) speed;
  position = 0;
}
