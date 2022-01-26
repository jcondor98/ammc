// AVR Multi Motor Control
// DC Motor test firmware
#include <util/delay.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"

#define DELAY_INTERVAL 3000

int main(void) {
  dcmotor_phy_pwm_init();
  while (1) {
    dcmotor_phy_load_speed_float(0);
    _delay_ms(DELAY_INTERVAL);
    dcmotor_phy_load_speed_float(DC_MOTOR_MAX_RPM_SPEED);
    _delay_ms(DELAY_INTERVAL);
    dcmotor_phy_load_speed_float(-DC_MOTOR_MAX_RPM_SPEED);
    _delay_ms(DELAY_INTERVAL);
    dcmotor_phy_load_speed_float(DC_MOTOR_MAX_RPM_SPEED / 2);
    _delay_ms(DELAY_INTERVAL);
    dcmotor_phy_load_speed_float(- DC_MOTOR_MAX_RPM_SPEED / 2);
    _delay_ms(DELAY_INTERVAL);
  }
}
