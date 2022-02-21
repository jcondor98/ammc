// AVR Multi Motor Control
/*! \file slave/source/dcmotor_phy.c
 * DC Motor Slave Hardware Abstraction Layer
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"
#include "dcmotor_phy_params.h"

#define abs(x) ((x) > 0 ? (x) : -(x))
#define min(x,y) ((x) < (y) ? (x) : (y))

typedef uint8_t duty_cycle_t;

namespace Phy = PhysicalLayer;


constexpr Phy::MotorPort::MotorPort(
    void (* const initialize)(void),
    void (* const loadSpeed)(dc_rpm_t speed),
    void (* const loadSpeedFloat)(float speed)
) : initialize(initialize),
    loadSpeed(loadSpeed),
    loadSpeedFloat(loadSpeedFloat) {}

static void initialize(void) {
  PWM_DDR |= PWM_DDR_MASK;
  PWM_TCCRA = PWM_TCCRA_VALUE;
  PWM_TCCRB = PWM_TCCRB_VALUE;
  DIRECTION_IN1_DDR |=  DIRECTION_IN1_DDR_MASK;
  DIRECTION_IN2_DDR |=  DIRECTION_IN2_DDR_MASK;
}

static inline void move_forward(void) {
  DIRECTION_IN1_PORT |=  DIRECTION_IN1_PORT_MASK;
  DIRECTION_IN2_PORT &= ~DIRECTION_IN2_PORT_MASK;
}

static inline void move_backward(void) {
  DIRECTION_IN1_PORT &= ~DIRECTION_IN1_PORT_MASK;
  DIRECTION_IN2_PORT |=  DIRECTION_IN2_PORT_MASK;
}

static inline void power_off_motor(void) {
  DIRECTION_IN1_PORT &= ~DIRECTION_IN1_PORT_MASK;
  DIRECTION_IN2_PORT &= ~DIRECTION_IN2_PORT_MASK;
}

static inline duty_cycle_t rpm2pwm(dc_rpm_t rpm) {
  static const double max_speed = DC_MOTOR_MAX_RPM_SPEED;
  static const double pwm_upper_bound = 0xFF;
  const double speed = rpm;
  return (duty_cycle_t) (speed / max_speed * pwm_upper_bound);
}

static inline dc_rpm_t bound_speed(dc_rpm_t rpm) {
  return min(rpm, DC_MOTOR_MAX_RPM_SPEED);
}

static void loadSpeed(dc_rpm_t speed) {
  duty_cycle_t duty_cycle = rpm2pwm(bound_speed(abs(speed)));
  if (speed > 0)
    move_forward();
  else if (speed < 0)
    move_backward();
  else power_off_motor();
  SPEED_REGISTER = duty_cycle;
}

static void loadSpeedFloat(float speed) {
  loadSpeed((dc_rpm_t) speed);
}


constexpr Phy::MotorPort defaultPort {
  initialize,
  loadSpeed,
  loadSpeedFloat,
};
