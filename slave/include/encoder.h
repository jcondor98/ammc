#pragma once
#include <stdint.h>
#include "common/dcmotor.h"

class TwoPhaseEncoder {
 public:
  constexpr TwoPhaseEncoder(void);

  int32_t getPosition(void);
  void updatePosition(bool phaseA, bool phaseB);
  dc_rpm_t sampleSpeed(void);

 private:
  int32_t ticksSinceLastSample;
};
