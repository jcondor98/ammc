// AVR Multi Motor Control
/*! \file slave/include/dcmotor_phy.h
 * DC Motor Slave Hardware Abstraction Layer
 *
 * \author Paolo Lucchesi
 */
#ifndef __DCMOTOR_PHYSICAL_HAL_H
#define __DCMOTOR_PHYSICAL_HAL_H
#include <stdint.h>
#include "common/dcmotor.h"

void dcmotor_phy_encoder_init(void);

void dcmotor_phy_pwm_init(void);

void dcmotor_phy_load_speed(dc_rpm_t speed);

void dcmotor_phy_load_speed_float(float speed);

uint8_t dcmotor_phy_read_encoder_phase_a(void);

uint8_t dcmotor_phy_read_encoder_phase_b(void);

void dcmotor_phy_pid_init(uint16_t pid_interval_msec);

void dcmotor_phy_pid_start(void);

void dcmotor_phy_pid_stop(void);

#endif  // __DCMOTOR_PHYSICAL_HAL_H
