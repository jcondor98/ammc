// AVR Multi Motor Control
/*! \file slave/source/dcmotor_phy_params.c
 * DC Motor Slave HAL hardware bindings and parameters, ATMega2560
 *
 * Used I/O pins are:
 * D53 (PORTB0/PCINT0) -> Encoder Phase A
 * D52 (PORTB1/PCINT1) -> Encoder Phase B
 * D13 (PORTD7/OCR0A)  -> PWM output
 * D4  (PORTG5/OCR0B)  -> Direction switch
 *
 * \author Paolo Lucchesi
 */
#ifndef __DCMOTOR_HAL_PARAMETERS_H
#define __DCMOTOR_HAL_PARAMETERS_H

// Register used to set the speed in RPM
#define SPEED_REGISTER_FW OCR0A
#define SPEED_REGISTER_BW OCR0B

// PWM Parameters
#define PWM_FW_DDR      DDRB
#define PWM_FW_DDR_MASK (1 << 7)
#define PWM_BW_DDR      DDRG
#define PWM_BW_DDR_MASK (1 << 5)
#define PWM_TCCRA       TCCR0A
#define PWM_TCCRA_VALUE ((1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1))
#define PWM_TCCRB       TCCR0B
#define PWM_TCCRB_VALUE ((1 << CS00))

// Encoder parameters
#define ENCODER_A_DDR       DDRB
#define ENCODER_A_DDR_MASK  (1 << 0)
#define ENCODER_A_PORT      PORTB
#define ENCODER_A_PORT_MASK ENCODER_A_DDR_MASK
#define ENCODER_A_PIN       PINB
#define ENCODER_A_PIN_MASK  ENCODER_A_DDR_MASK
#define ENCODER_B_DDR       DDRB
#define ENCODER_B_DDR_MASK  (1 << 1)
#define ENCODER_B_PORT      PORTB
#define ENCODER_B_PORT_MASK ENCODER_B_DDR_MASK
#define ENCODER_B_PIN       DDRB
#define ENCODER_B_PIN_MASK  ENCODER_B_DDR_MASK
#define ENCODER_PCICR       PCICR
#define ENCODER_PCICR_MASK  (1 << PCIE0)
#define ENCODER_PCMSK       PCMSK0
#define ENCODER_PCMSK_MASK  (1 << 0)

// Sampling timer parameters
#define SAMPLING_TCCRA TCCR2A
#define SAMPLING_TCCRA_VALUE 0
#define SAMPLING_TCCRB TCCR2B
#define SAMPLING_TCCRB_VALUE ((1 << WGM22) | (1 << CS20) | (1 << CS22))

// PID controller parameters
#define PID_TCNT TCNT1
#define PID_TIMSK TIMSK1
#define PID_TIMSK_MASK (1 << OCIE1A)

#endif  // __DCMOTOR_HAL_PARAMETERS_H
