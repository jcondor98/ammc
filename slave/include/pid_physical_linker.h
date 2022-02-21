#pragma once
#include <stdint.h>
#include "dcmotor.h"

namespace PidPhysicalLinker {
  /*!
   * Initialize the Pid controller linker with its timer
   *
   * @param intervalInMillis The PID controller timer trigger interval
   */
  void initialize(uint16_t intervalInMillis);

  //! Periodically adjust the speed of a motor
  void linkMotor(DCMotor *motorToLink);

  //! Start the PID physical linker periodic activity
  void start(void);

  //! Stop the PID physical linker periodic activity
  void stop(void);
};
