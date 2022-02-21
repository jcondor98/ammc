// AVR Multi Motor Control
/*! \file slave/include/dcmotor.h
 * DC Motor Slave Module
 *
 * \author Paolo Lucchesi
 */
#pragma once
#include <stdint.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"
#include "dcmotor_pid.h"

class DCMotor {
 public:
  DCMotor(void);

  //! Initialize the physical layer for all motors
  static void initializePhysicalLayer(void);

  //! @return The last sampled speed (internally stored)
  dc_rpm_t getActualSpeed(void);

  //! Set a new speed (without applying it)
  void setTargetSpeed(dc_rpm_t);

  //! Apply the previously given RPM value
  void applyTargetSpeed(void);

  //! Sample the actual speed
  dc_rpm_t sampleActualSpeed(void);

  //! Adjust the actual speed using the PID controller
  void adjustActualSpeed(void);

  //! Start the PID controller activity
  void startPid(void);

  //! Stop the PID controller activity
  void stopPid(void);

  //! Read the encoder value(s) and update the current motor position
  void updatePosition(void);

 private:
  PhysicalLayer::MotorPort &phy;
  PidController pid;
  TwoPhaseEncoder encoder;
  volatile dc_rpm_t actualSpeed;
  dc_rpm_t targetSpeed;
  dc_rpm_t nextSpeed;
};
