// AVR Multi Motor Control
/*! \file slave/include/dcmotor_phy.h
 * DC Motor Slave Hardware Abstraction Layer
 *
 * \author Paolo Lucchesi
 */
#pragma once
#include <stdint.h>
#include "common/dcmotor.h"

namespace PhysicalMotor {
  void initializePWM(void);
  void loadSpeed(dc_rpm_t speed);
  void loadSpeed(float speed);

  void initializeEncoder(void);
  uint8_t readEncoderPhaseA(void);
  uint8_t readEncoderPhaseB(void);

  void initializePid(uint16_t pid_interval_msec);
  void startPid(void);
  void stopPid(void);
};
