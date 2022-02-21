// AVR Multi Motor Control
/*! \file slave/include/dcmotor_phy.h
 * DC Motor Slave Hardware Abstraction Layer
 *
 * \author Paolo Lucchesi
 */
#pragma once
#include <stdint.h>
#include "common/dcmotor.h"

namespace PhysicalLayer {
  class MotorPort {
   public:
    constexpr MotorPort(
        void (* const initialize)(void),
        void (* const loadSpeed)(dc_rpm_t speed),
        void (* const loadSpeedFloat)(float speed)
    );

    void (* const initialize)(void);
    void (* const loadSpeed)(dc_rpm_t speed);
    void (* const loadSpeedFloat)(float speed);
  };

  extern const MotorPort defaultPort;
};
