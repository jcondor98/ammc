#include "encoder.h"
#define ONE_MINUTE 60000

constexpr TwoPhaseEncoder::TwoPhaseEncoder(void)
  : ticksSinceLastSample(0) {}

int32_t TwoPhaseEncoder::getPosition(void) {
  return ticksSinceLastSample % DC_ENC_SIGNALS_PER_ROUND;
}

void TwoPhaseEncoder::updatePosition(bool phaseA, bool phaseB) {
  if (phaseA) phaseB ? ++ticksSinceLastSample : --ticksSinceLastSample;
}

dc_rpm_t TwoPhaseEncoder::sampleSpeed(void) {
  dc_rpm_t speed = ticksSinceLastSample / DC_ENC_SIGNALS_PER_ROUND
    * ONE_MINUTE / DC_SAMPLING_INTERVAL;
  ticksSinceLastSample = 0;
  return speed;
}
